# FAQ: Redis Cluster PVC 卡在 Pending（StorageClass 不存在 + 节点磁盘满）

> 场景：`kubectl describe pvc redis-data-redis-cluster-0` 报 `storageclass "standard" not found`，Pod 也无法调度。
> 本文记录完整排查过程与根因分析。

---

## 1. 问题现象

```console
$ kubectl describe pvc redis-data-redis-cluster-0
Name:          redis-data-redis-cluster-0
Namespace:     default
StorageClass:  standard
Status:        Pending
Volume:
Events:
  Type     Reason              Age                 From                         Message
  ----     ------              ----               ----                         -------
  Warning  ProvisioningFailed  3s (x10 over 2m17s) persistentvolume-controller  storageclass.storage.k8s.io "standard" not found
```

同时 Pod 一直处于 Pending：

```console
$ kubectl get pods -l app=redis-cluster
NAME              READY   STATUS    RESTARTS   AGE
redis-cluster-0   0/1     Pending   0          6m42s
```

---

## 2. 排查过程（证据链）

### 2.1 第一层：StorageClass 名不匹配

`storageclass "standard" not found` 的直接原因是 StatefulSet 的 `volumeClaimTemplates` 里写死了不存在的 StorageClass：

```yaml
# redis-cluster-statefulset.yaml
volumeClaimTemplates:
- metadata:
    name: redis-data
  spec:
    accessModes:
    - ReadWriteOnce
    storageClassName: standard  # ← 集群里不存在
```

验证集群实际可用的 StorageClass：

```console
$ kubectl get storageclass
NAME                   PROVISIONER             RECLAIMPOLICY   VOLUMEBINDINGMODE      ALLOWVOLUMEEXPANSION   AGE
local-path (default)   rancher.io/local-path   Delete          WaitForFirstConsumer   false                  139d
```

集群只有 `local-path`，没有 `standard`。

### 2.2 第二层：修改 YAML 后 PVC 依然 Pending —— 判断这不是病根

将 `standard` 改为 `local-path` 并重建 StatefulSet 后，PVC 仍然 Pending，但这**不能直接推断还有问题**：

- `local-path` 的 `VOLUMEBINDINGMODE` 是 **WaitForFirstConsumer**：PVC 要等一个使用它的 Pod 先被调度到节点上，才会真正创建 PV。
- 因此 **PVC Pending 可能是结果，而不是原因**。

关键要看 Pod 是否能被调度。

### 2.3 第三层：真正的线索在 Pod

```console
$ kubectl get pods -l app=redis-cluster -o wide
NAME              READY   STATUS    RESTARTS   AGE   IP       NODE     NOMINATED NODE
redis-cluster-0   0/1     Pending   0          68s   <none>   <none>   <none>
```

`NODE: <none>` 说明 Pod **根本没被调度器放到任何节点**。调度器拒绝是有日志的：

```console
$ kubectl describe pod redis-cluster-0 | tail
Events:
  Type     Reason            Age   From               Message
  ----     ------            ----  ----               -------
  Warning  FailedScheduling  23s   default-scheduler  0/1 nodes are available: 1 node(s) had untolerated taint(s)
```

### 2.4 第四层：调度器的 taintToleration 过滤逻辑

调度器有一个 `taintToleration` 过滤阶段：**只要节点存在 effect 为 `NoSchedule` 的污点，而 Pod 没有匹配的 toleration，该节点就被直接过滤**。集群只有 1 个节点，于是 `0/1 nodes are available`。

查看节点污点：

```console
$ kubectl get nodes -o jsonpath='{range .items[*]}{.metadata.name}{"\t"}{.spec.taints}{"\n"}{end}'
jihan	[{"effect":"NoSchedule","key":"node.kubernetes.io/disk-pressure","timeAdded":"2026-08-13T02:24:32Z"}]
```

### 2.5 第五层：确认污点不是手动加的，也删不掉

`node.kubernetes.io/disk-pressure` 是 **kubelet 自动维护的污点**：kubelet 监测到本机磁盘条件异常时会自动打在 Node 对象上。验证污点背后有真实的节点条件：

```console
$ kubectl get node jihan -o jsonpath='{range .status.conditions[*]}{.type}={.status} ({.reason}){""}{"\n"}{end}'
MemoryPressure=False (KubeletHasSufficientMemory)  kubelet has sufficient memory available
DiskPressure=True (KubeletHasDiskPressure)        kubelet has disk pressure        ← 磁盘压力
Ready=True (KubeletReady)                         kubelet is posting ready status
```

```console
$ df -h /
Filesystem                        Size  Used Avail Use% Mounted on
/dev/mapper/ubuntu--vg-ubuntu--lv  97G   93G     0 100% /
```

根分区 **100% 满，剩余 0**。

---

## 3. 完整因果链

```
根分区 100% 满
  → kubelet 上报节点条件 DiskPressure=True
  → kubelet 自动打上 node.kubernetes.io/disk-pressure:NoSchedule 污点
  → 调度器 taintToleration 过滤掉唯一节点（Pod 无对应 toleration）
  → Pod redis-cluster-0 无法调度（NODE: <none>）
  → WaitForFirstConsumer 模式下 PVC 等不到消费者 → 保持 Pending
```

⚠️ **为什么不能靠删污点解决**：手动 `kubectl taint node jihan node.kubernetes.io/disk-pressure-` 只能临时生效，kubelet 检测到磁盘仍满，几秒内会重新打上。治本必须清理磁盘。

---

## 4. 解决方案

### 4.1 修正 StorageClass（治标）

```yaml
# redis-cluster-statefulset.yaml
storageClassName: local-path   # 根据你的集群修改
```

注意两个 Kubernetes 限制：

1. **PVC 是独立资源**：改 StatefulSet 模板不会更新已存在的 PVC，需删除旧 PVC 让它按新模板重建（从未绑定过则无数据损失）。
2. **`volumeClaimTemplates` 不允许就地更新**：必须先删 StatefulSet 再 apply，否则报
   `Forbidden: updates to statefulset spec for fields other than ... are forbidden`。

重建步骤：

```console
# 删除旧的 StatefulSet（级联删除其 Pod 与 PVC）
$ kubectl delete sts redis-cluster
$ kubectl apply -f redis-cluster-statefulset.yaml

# 若存在无 ownerReference 的残留 PVC（删除 sts 过程中产生），需手动再删一次
$ kubectl delete pvc redis-data-redis-cluster-0
```

验证 PVC 已使用正确 StorageClass：

```console
$ kubectl get pvc -l app=redis-cluster -o custom-columns=NAME:.metadata.name,STATUS:.status.phase,SC:.spec.storageClassName
NAME                         STATUS    SC
redis-data-redis-cluster-0   Pending   local-path    # 正确；WaitForFirstConsumer 下需等 Pod 调度后才绑定
```

### 4.2 清理磁盘（治本）

```console
# 查看可回收空间
$ docker system df
TYPE            TOTAL     ACTIVE    SIZE      RECLAIMABLE
Images          17        17        20.57GB   0B (0%)
Containers      22        22        321.8MB   0B (0%)
Local Volumes   13        2         1.489GB   1.44GB (96%)
Build Cache     44        0         2.444GB   1.061GB

# 清理未使用的 build cache / 悬空镜像 / 日志等，释放几个 GB 后污点自动消失
$ docker builder prune
$ docker image prune
```

磁盘释放后，kubelet 的 `DiskPressure` 条件恢复为 False，污点自动移除，Pod 正常调度，PVC 随之绑定。

---

## 5. 排查方法论总结

1. **先看表象，再挖根因**：`describe` 报的错往往只是最表层，修完可能还有下一层。
2. **区分"结果"与"原因"**：WaitForFirstConsumer 模式下 PVC Pending 是正常状态，要顺着 Pod → 调度器 → 节点逐层向上查。
3. **调度器日志是定位利器**：`kubectl describe pod` 的 `FailedScheduling` 事件直接告诉你为什么没有可用节点。
4. **污点 → 节点条件 → 实际资源** 三级验证：先看污点，再确认污点对应节点条件，最后用 `df` 等命令验证物理事实。
5. **区分"手动污点"和"自动污点"**：`node.kubernetes.io/*` 系污点由 kubelet 维护，删了会自动回来，必须解决背后的问题。