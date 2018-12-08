
DIR_INC = .
DIR_SRC = .
DIR_OBJ = .
DIR_BIN = .

NCF_LIB = /GPFS/cess/JiHan/Toollib/netcdf/lib
NCF_INC = /GPFS/cess/JiHan/Toollib/netcdf/include
PNCF_LIB = /GPFS/cess/gaoyuan/pnetcdf/lib
PNCF_INC =/GPFS/cess/gaoyuan/pnetcdf/include
NCL_INC = /GPFS/cess/gaoyuan/ncl/include 
NCL_LIB = /GPFS/cess/gaoyuan/ncl/lib
HDF5_INC = /GPFS/cess/JiHan/Toollib/hdf5-1.8.20/include 
HDF5_LIB = /GPFS/cess/JiHan/Toollib/hdf5-1.8.20/lib
CAIRO_LIB=/GPFS/cess/syang/cairo-1.14.10-install/lib

ALL_LD = -L${PNCF_LIB} -L${NCL_LIB} -L${CAIRO_LIB} -L${HDF5_LIB} -L${NCF_LIB}
ALL_INC = -I${PNCF_INC} -I${NCL_INC} -I${HDF5_INC} -I${NCF_INC}
NCAR_LLIB = -lnetcdf -lcurl -lssl -lidn -lrt -lldap -lhdf5_hl -lhdf5 -lsz -lhlu -lncarg -lncarg_gks -lncarg_c -lX11 -lXext -lcairo -lfontconfig -lpixman-1 -lfreetype -lexpat -lpng -lz -lpthread -lXrender -lbz2 -lgfortran -lm
ALL_LIB = -lpnetcdf ${NCAR_LLIB}

CXX = mpiicxx
CC = gcc
CFLAGS = -g -O2 -fPIC -fopenmp
MPIF90 = mpiif90

SRC = $(wildcard ${DIR_SRC}/*.c)
OBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TARGET = a.out

all:
BIN_TARGET = ${DIR_BIN}/${TARGET}

${BIN_TARGET}: ${OBJ}
	$(CC) $(CFLAGS) $(OBJ)  -o $@ $(ALL_LD) $(ALL_LIB)

${DIR_OBJ}/%.o:${DIR_SRC}/%.c
	$(CC) $(CFLAGS)  -c $< -o $@ $(ALL_INC)

.PHONY:clean
clean:
	-rm -f *.out
	-rm -f *.o
#	-rm -f ./ncfile/*.nc


fixed=xy04c
.PHONY:${fixed}
${fixed}: ${fixed}.c
	$(CC) $(CFLAGS) -o $@.out $< $(ALL_LD) $(ALL_INC) $(ALL_LIB)

IN=$(in)
.PHONY:one
one: ${IN}.c
	$(CC) $(CFLAGS) -o ${IN}.out $< $(ALL_LD) $(ALL_INC) $(ALL_LIB)

.PHONY:maketest
maketest:
	-echo ${SRC}
	-echo ${OBJ}
	-echo $(notdir ${SRC})

#（1）Makefile中的 符号 $@, $^, $< 的意思：
#　　$@  表示目标文件
#　　$^  表示所有的依赖文件
#　　$<  表示第一个依赖文件
#　　$?  表示比目标还要新的依赖文件列表

#（2）wildcard、notdir、patsubst的意思：
#　　wildcard : 扩展通配符
#　　notdir ： 去除路径
#　　patsubst ：替换通配符

#（3）PHONY 表示伪目标，比如clean，如果文件中
#	  存在clean文件，那么添加了此关键字依旧能
#	  执行rm操作。


