#core/src/Filelists.mk:
#	git submodule init
#	git submodule update
LWIPDIR=core/src/
include $(kbuild-dir)/core/src/Filelists.mk
lwip-core-objs = $(subst .c,.o, $(filter-out $(LWIPDIR)/core/mem.c, $(COREFILES)) $(LWIPDIR)/netif/ethernet.c) sys_arch.o net.o
lwip-ipv4-objs = $(subst .c,.o, $(CORE4FILES))
lwip-ipv6-objs = $(subst .c,.o, $(CORE6FILES))
lwip-api-objs = $(subst .c,.o, $(APIFILES))
lwip-ppp-objs = $(subst .c,.o, $(PPPFILES))
lwip-http-objs = $(subst .c,.o, $(HTTPDFILES))
lwip-iperf-objs = $(subst .c,.o, $(LWIPERFFILES))
lwip-sntp-objs = $(subst .c,.o, $(SNTPFILES))
lwip-netbios-objs = $(subst .c,.o, $(NETBIOSNSFILES))

obj-y += lwip-core.o lwip-api.o
obj-$(CONFIG_LWIP_MEM) += $(LWIPDIR)/core/mem.o
obj-$(CONFIG_MEM_LIBC_MALLOC_ALGIN) += mem-aligned.o
obj-$(CONFIG_LWIP_IPV4) += lwip-ipv4.o
obj-$(CONFIG_LWIP_IPV6) += lwip-ipv6.o
obj-$(CONFIG_LWIP_PPP) += lwip-ppp.o
obj-$(CONFIG_LWIP_HTTP) += lwip-http.o
obj-$(CONFIG_LWIP_IPERF) += lwip-iperf.o
obj-$(CONFIG_LWIP_SNTP) += lwip-sntp.o
obj-$(CONFIG_LWIP_NETBIOS) += lwip-netbios.o

subdir-ccflags-y=-I$(kbuild-dir)/include
