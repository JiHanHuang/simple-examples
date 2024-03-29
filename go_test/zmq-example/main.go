package main

import (
	"bytes"
	"encoding/binary"
	"fmt"

	zmq "github.com/pebbe/zmq4"
	"inet.af/netaddr"
)

var StreamDemo = []byte{
	0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x03, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x63, 0xf8, 0x64, 0x97,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xae, 0xd4,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x01, 0x04, 0x8d, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x04, 0x8d, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x45, 0x00, 0x00, 0x84, 0x03, 0xe8,
	0x00, 0x00, 0x40, 0x11, 0x5a, 0x75, 0x8d, 0x04,
	0x01, 0x03, 0x8d, 0x04, 0x01, 0x01, 0x08, 0x68,
	0x08, 0x68, 0x00, 0x70, 0x69, 0xcb, 0x30, 0xff,
	0x00, 0x60, 0x00, 0x00, 0x9c, 0x41, 0x60, 0x00,
	0x00, 0x00, 0x00, 0x38, 0x3a, 0xff, 0xfe, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0b,
	0xdb, 0xff, 0xfe, 0x91, 0x6f, 0xd6, 0xff, 0x02,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0x00,
	0xd9, 0x2a, 0x00, 0x00, 0x07, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x05, 0xdc, 0x03, 0x04,
	0x40, 0x40, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01, 0x41,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
}

func main() {
	// raw, err := ParseRawStream(StreamDemo)
	// if err != nil {
	// 	fmt.Printf("Err:%v", err)
	// 	return
	// }
	// fmt.Printf("%+v", raw)
	sendto, err := zmq.NewSocket(zmq.PUSH)
	if err != nil {
		fmt.Printf("Err:%v", err)
		return
	}
	defer sendto.Close()
	backend.Connect("ipc:///var/run/track/track_agent.ipc")
	keyBytes := []byte{}
	keyBytes = binary.BigEndian.AppendUint32(keyBytes, 1)
	sendto.SendBytes(keyBytes, zmq.SNDMORE|zmq.DONTWAIT)
	sendto.SendBytes(v1.StreamDemo, zmq.DONTWAIT)
	fmt.Println("send over.")
}

const (
	DATA_HEAD_LEN = 138
)

type Raw struct {
	Version       uint32
	Direction     uint8
	MsgTypevalue  uint8
	InterfaceRole uint8
	CauseValue    uint8
	TvSec         uint64
	TvUsec        uint64
	SupiImsi      uint64
	GpsiMsisdn    uint64
	SrcAddr       netaddr.IP
	DstAddr       netaddr.IP
	Family        uint8
	DnnName       *string
	L2Head        uint8
	DataLen       uint16
	Data          []byte
}

func (r *Raw) IsV4() bool {
	return r.Family == 1
}

func ParseRawStream(stream []byte) (*Raw, error) {
	if len(stream) < DATA_HEAD_LEN {
		return nil, fmt.Errorf("illegal stream len %d", len(stream))
	}
	ret := &Raw{}
	dataIndex := 0
	// Version
	ret.Version = binary.BigEndian.Uint32(stream[dataIndex:])
	dataIndex += 4
	// Direction
	ret.Direction = stream[dataIndex]
	dataIndex += 1
	// MsgTypevalue
	ret.MsgTypevalue = stream[dataIndex]
	dataIndex += 1
	// InterfaceRole
	ret.InterfaceRole = stream[dataIndex]
	dataIndex += 1
	// CauseValue
	ret.CauseValue = stream[dataIndex]
	dataIndex += 1
	// Time
	ret.TvSec = binary.BigEndian.Uint64(stream[dataIndex:])
	dataIndex += 8
	ret.TvUsec = binary.BigEndian.Uint64(stream[dataIndex:])
	dataIndex += 8
	// SupiImsi
	ret.SupiImsi = binary.BigEndian.Uint64(stream[dataIndex:])
	dataIndex += 8
	// GpsiMsisdn
	ret.GpsiMsisdn = binary.BigEndian.Uint64(stream[dataIndex:])
	dataIndex += 8
	// src addr
	srcAddrData := stream[dataIndex : dataIndex+16]
	dataIndex += 16
	// dst addr
	dstAddrData := stream[dataIndex : dataIndex+16]
	dataIndex += 16
	// family
	ret.Family = stream[dataIndex]
	dataIndex += 1
	if ret.Family == 1 {
		// ipv4
		srcAddrArray := [4]byte{}
		copy(srcAddrArray[:], srcAddrData)
		dstAddrArray := [4]byte{}
		copy(dstAddrArray[:], dstAddrData)
		ret.SrcAddr = netaddr.IPFrom4(srcAddrArray)
		ret.DstAddr = netaddr.IPFrom4(dstAddrArray)
	} else if ret.Family == 2 {
		// ipv6
		srcAddrArray := [16]byte{}
		copy(srcAddrArray[:], srcAddrData)
		dstAddrArray := [16]byte{}
		copy(dstAddrArray[:], dstAddrData)
		ret.SrcAddr = netaddr.IPFrom16(srcAddrArray)
		ret.DstAddr = netaddr.IPFrom16(dstAddrArray)
	} else {
		return nil, fmt.Errorf("illegal family %d", stream[dataIndex])
	}
	//DnnNmae
	dnnBytes := stream[dataIndex : dataIndex+64]
	dnnBytes = bytes.TrimRight(dnnBytes, "\x00")
	dnnName := string(dnnBytes)
	ret.DnnName = &dnnName
	dataIndex += 64
	// l2_head
	ret.L2Head = stream[dataIndex]
	dataIndex += 1
	// // dataLen
	// ret.DataLen = binary.BigEndian.Uint16(stream[dataIndex:])
	// dataIndex += 2
	// stream
	// ret.Data = stream[dataIndex:ret.DataLen]
	ret.Data = stream[dataIndex:]
	return ret, nil
}
