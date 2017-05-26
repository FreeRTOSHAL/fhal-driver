#include <net/net.h>
#define NET_PRV
#include <net/net_prv.h>
#include <net/mac.h>
#include <rtc.h>
#include <stdint.h>
#include <string.h>
struct uint1_t {
	uint8_t data:1;
} PACKED;
typedef struct uint1_t uint1_t;
#define UINT1_T_GET_VALUE(x) ((x).data)
#define UINT1_T_SET_VALUE(x, v) ((x).data = ((v) & 0x1))
struct uint4_t {
	uint8_t data:4;
} PACKED;
typedef struct uint4_t uint4_t;
#define UINT4_T_GET_VALUE(x) ((x).data)
#define UINT4_T_SET_VALUE(x, v) ((x).data = (v))
struct uint48_t {
	uint64_t data:48;
} PACKED;
typedef struct uint48_t uint48_t;
#define UINT48_T_GET_VALUE(x) ((x).data)
#define UINT48_T_SET_VALUE(x, v) ((x).data = (v))
#define swap48(d) ({ \
	union {uint48_t ret; uint8_t ret8[48];} tmp;\
	tmp.ret = d; \
	/* swap 012345 -> 543210 */ \
	tmp.ret8[0] ^= tmp.ret8[5]; \
	tmp.ret8[5] ^= tmp.ret8[0]; \
	tmp.ret8[0] ^= tmp.ret8[5]; \
	tmp.ret8[1] ^= tmp.ret8[4]; \
	tmp.ret8[4] ^= tmp.ret8[1]; \
	tmp.ret8[1] ^= tmp.ret8[4]; \
	tmp.ret8[2] ^= tmp.ret8[3]; \
	tmp.ret8[3] ^= tmp.ret8[2]; \
	tmp.ret8[2] ^= tmp.ret8[3]; \
	tmp.ret; \
})
#define be48_to_cpu(d) swap48(d)
#define cpu_to_be48(d) swap48(d)
typedef uint1_t Boolean;
typedef uint4_t Enumeration4;
typedef uint8_t Enumeration8;
typedef uint16_t Enumeration16;
typedef uint4_t UInteger4;
typedef int8_t Integer8;
typedef uint8_t UInteger8;
typedef int8_t Integer16;
typedef uint16_t UInteger16;
typedef int32_t Integer32;
typedef uint32_t UInteger32;
typedef uint48_t UInteger48;
typedef int64_t Integer64;
typedef uint4_t Nibble;
typedef uint8_t Octet;
/* 
 * PTP Datatype
 */
/* sec are 48 Bit Wide */
struct ptp_TimeInterval {
	Integer64 scaledNanoseconds;
} PACKED;
struct ptp_Timestamp {
	UInteger48 sec;
	UInteger32 nsec;
} PACKED;
struct ptp_ClockIdentity {
	Octet id[8];
} PACKED;
struct ptp_PortIdentity {
	struct ptp_ClockIdentity clockIdentity;
	UInteger16 portNumber;
} PACKED;
struct ptp_PortAddress {
	Enumeration16 networkProtocol;
	UInteger16 addressLength;
	Octet addressField[6];
} PACKED;
struct ptp_ClockQuality {
	UInteger8 clockClass;
	Enumeration8 clockAccuracy;
	UInteger16 offsetScaledLogVariance;
} PACKED;
struct ptp_TLV {
	Enumeration16 tlvType;
	UInteger16 lengthField;
	Octet valueField[0];
} PACKED;
struct ptp_PTPText {
	UInteger8 lengthField;
	Octet textField[0];
} PACKED;
struct ptp_FaultRecord {
	UInteger16 faultRecordLength;
	struct ptp_Timestamp faultTime;
	Enumeration8 severityCode;
	struct ptp_PTPText faultName;
	struct ptp_PTPText faultValue;
} PACKED;
/*
 * PTP DataSets
 */
struct ptp_defaultDS {
	bool twoStepFlag;
	struct ptp_PortIdentity clockIdentity;
	UInteger16 numberPorts;
	struct ptp_ClockQuality clockQuality;
	UInteger8 priority1;
	UInteger8 priority2;
	UInteger8 domainNumber;
	bool slaveOnly;
};
struct ptp_currentDS {
	UInteger16 stepsRemoved;
	Integer64 offsetFromMaster;
	Integer64 meanPathDelay;
};
struct ptp_parentDS {
	struct ptp_PortIdentity parentPortIdentity;
	bool parentStats;
	UInteger16 observedParentOffsetScaledLogVariance;
	Integer32 observedParentClockPhaseChangeRate;
	struct ptp_PortIdentity grandmasterIdentity;
	struct ptp_ClockQuality grandmasterClockQuality;
	UInteger8 grandmasterPriority1;
	UInteger8 grandmasterPriority2;
};
struct ptp_timePropertiesDS {
	UInteger16 currentUtcOffset;
	bool currentUtcOffsetValid;
	bool leap59;
	bool leap61;
	bool timeTraceable;
	bool frequencyTraceable;
	bool ptpTimescale;
	Enumeration8 timeSource;
};
struct ptp_portDS {
	struct ptp_PortAddress portIdentity;
	Enumeration8 portState;
	Integer8 logMinDelayReqInterval;
	struct ptp_TimeInterval peerMeanPathDelay;
	Integer8 logAnnounceInterval;
	UInteger8 announceReceiptTimeout;
	Integer8 logSyncInterval;
	Enumeration8 delayMechanism;
	Integer8 logMinPdelayReqInterval;
	uint8_t versionNumber;

};
struct ptp_header {
	Nibble transportSpecific;
	Enumeration4 messageType;
	UInteger4 reserved;
	UInteger4 versionPTP;
	UInteger16 messageLength;
	UInteger8 domainNumber;
	Octet flagField[2];
	Integer64 correctionField;
	struct ptp_PortIdentity sourcePortIdentity;
	UInteger16 sequenceId;
	UInteger8 controlField;
	Integer8 logMessageInterval;
	UInteger8 payload[0];
} PACKED;
struct ptp_sync {
	struct ptp_header header;
	struct ptp_Timestamp timestamp;
} PACKED;
struct ptp_announce {
	struct ptp_header header;
	struct ptp_Timestamp timestamp;
	Integer16 currentUtcOffset;
	UInteger8 grandmasterPriority1;
	struct ptp_ClockQuality grandmasterClockQuality;
	UInteger8 grandmasterPriority2;
	struct ptp_ClockIdentity grandmasterIdentity;
	UInteger16 stepsRemoved;
	Enumeration8 timeSource;
} PACKED;
/**
 * PTP State after 9.2.5 State machines IEC 61588
 */
enum ptp_state {
	INITIALIZING = 0,
	FAULTY,
	DISABLED,
	LISTENING,
	/* TODO not used */
	PRE_MASTER,
	/* TODO not used */
	MASTER,
	PASSIVE,
	UNCALIBRATED,
	SLAVE
};
struct net_ptp {
	struct net_generic gen;
	struct rtc *clock;
	enum ptp_state state;
	struct ptp_defaultDS defaultDS;
	struct ptp_currentDS currentDS;
	struct ptp_parentDS parentDS;
	struct ptp_timePropertiesDS timePropertiesDS;
	struct ptp_portDS portDS;
};
#define VLAN_ID 0x8100
#define VLAN_PACKET_LEN 0x4
#define PTP_ID 0x88F7
#define PTP_ID_LEN 0x2
#define PTP_MESSAGE_TYPE_SYNC 0x0
#define PTP_MESSAGE_TYPE_DELAY_REQ 0x1
#define PTP_MESSAGE_TYPE_PDELAY_REQ 0x2
#define PTP_MESSAGE_TYPE_PDELAY_RESP 0x3
#define PTP_MESSAGE_TYPE_FOLLOW_UP 0x8
#define PTP_MESSAGE_TYPE_DELAY_RESP 0x9
#define PTP_MESSAGE_TYPE_PDELAY_RESP_FOLLOW_UP 0xA
#define PTP_MESSAGE_TYPE_ANNOUNCE 0xB
#define PTP_MESSAGE_TYPE_SIGNALING 0xC
#define PTP_MESSAGE_TYPE_MANAGEMENT 0xD

#define PRINTF(fmt, ...) printf("PTP: " fmt, __VA_ARGS__)
static char *ptp_stateString[] = {
	"INITIALIZING",
	"FAULTY",
	"DISABLED",
	"LISTENING",
	"PRE_MASTER",
	"MASTER",
	"PASSIVE",
	"UNCALIBRATED",
	"SLAVE"
};
static void timestampToTimespec(struct ptp_Timestamp *timestamp, struct timespec *time) {
	CONFIG_ASSERT(sizeof(time_t) == 8);
	UInteger48 tmp = be48_to_cpu(timestamp->sec);
	time->tv_sec = UINT48_T_GET_VALUE(tmp);
	time->tv_nsec = be32_to_cpu(timestamp->nsec);
}
static void setState(struct net_ptp *net, enum ptp_state state) {
	if (net->state != state) {
		PRINTF("%s -> %s\n", ptp_stateString[net->state], ptp_stateString[state]);
		net->state = state;
		net->portDS.portState = net->state;
	}
}
static void stop(struct net_ptp *net) {
	/* disconect */
	setState(net, DISABLED);
}
static void start(struct net_ptp *net) {
	setState(net, INITIALIZING);
	/* we support twoStep */
	net->defaultDS.twoStepFlag = true;
	/* TODO SlaveOnly */
	net->defaultDS.slaveOnly =true;
	/* slave only 255 as specified in 7.6.2.4 */
	net->defaultDS.clockQuality.clockClass = 255;
	net->defaultDS.domainNumber = 0;

	net->currentDS.stepsRemoved = 0;
	net->currentDS.offsetFromMaster = 0;
	net->currentDS.meanPathDelay = 0;

	memset(&net->parentDS, 0, sizeof(struct ptp_parentDS));
	net->parentDS.parentPortIdentity = net->defaultDS.clockIdentity;
	net->parentDS.parentStats = false;
	net->parentDS.grandmasterIdentity = net->defaultDS.clockIdentity;
	net->parentDS.grandmasterClockQuality = net->defaultDS.clockQuality;
	net->parentDS.grandmasterPriority1 = net->defaultDS.priority1;
	net->parentDS.grandmasterPriority2 = net->defaultDS.priority2;


	net->portDS.portIdentity.networkProtocol = 0;
	net->portDS.portIdentity.addressLength = 6;
	/* TODO get Mac Adress from mac layer */
	net->portDS.portIdentity.addressField[0] = 0x0;
	net->portDS.portIdentity.addressField[1] = 0x12;
	net->portDS.portIdentity.addressField[2] = 0x34;
	net->portDS.portIdentity.addressField[3] = 0x56;
	net->portDS.portIdentity.addressField[4] = 0x78;
	net->portDS.portIdentity.addressField[5] = 0x9A;
	net->portDS.portState = net->state;
	net->portDS.logMinDelayReqInterval = 0;
	net->portDS.peerMeanPathDelay.scaledNanoseconds = 0;
	net->portDS.logAnnounceInterval = 0;
	net->portDS.announceReceiptTimeout = 0;
	net->portDS.logSyncInterval = 0;
	net->portDS.delayMechanism = 0;
	net->portDS.logMinPdelayReqInterval = 0;
	net->portDS.versionNumber = 2;

	/* reset to LISTENING and wait for sync msg */
	setState(net, LISTENING);
}
static void handle_sync(struct net_ptp *net, struct ptp_header *ptp) {
	switch (net->state) {
		case UNCALIBRATED:
			/* fall through */
		case SLAVE:
			
			break;
		/* never in this state */
		case INITIALIZING:
		case PRE_MASTER:
		case MASTER:
			setState(net, FAULTY);
			break;
		/* ignore Message in this mode */
		case PASSIVE:
		case LISTENING:
		case DISABLED:
		case FAULTY:
			break;
	}
}
static void handle_followUp(struct net_ptp *net, struct ptp_header *ptp) {
	switch (net->state) {
		case UNCALIBRATED:
			/* fall through */
		case SLAVE:
			break;
		/* never in this state */
		case INITIALIZING:
		case PRE_MASTER:
		case MASTER:
			setState(net, FAULTY);
			break;
		/* ignore Message in this mode */
		case PASSIVE:
		case LISTENING:
		case DISABLED:
		case FAULTY:
			break;
	}
}

static void handle_announce(struct net_ptp *net, struct ptp_header *ptp) {
	struct ptp_announce *ann = (struct ptp_announce *) ptp->payload;
	switch (net->state) {
		case LISTENING:
			break;
		case UNCALIBRATED:
			/* fall through */
		case SLAVE:
			break;
		/* never in this state */
		case INITIALIZING:
		case PRE_MASTER:
		case MASTER:
			setState(net, FAULTY);
			break;
		/* ignore Message in this mode */
		case PASSIVE:
		case DISABLED:
		case FAULTY:
			break;
	}
	
}

NET_INIT(ptp, index) {
	int32_t ret;
	struct net *n = NET_GET_DEV(index);
	struct net_ptp *net = (struct net_ptp *) n;
	if  (net == NULL) {
		return NULL;
	}
	ret = net_genericInit(n);
	if (ret < 0) {
		return NULL;
	}
	if (ret == NET_ALREDY_INITED) {
		return n;
	}
	/* without Clock aways Disabled */
	setState(net, DISABLED);

	return n;
}
int32_t ptpd_connect(struct net *n, struct rtc *rtc) {
	struct net_ptp *net = (struct net_ptp *) n;
	setState(net, INITIALIZING);
	/* init */
	net->clock = rtc;
	/* wait for up signal from Hardware */
	stop(net);
	return 0;
}
NET_DEINIT(ptp, net) {
	return 0;
}
NET_ALLOC_NETBUFF(ptp, net, size) {
	struct net *child = net_getChild(net);
	return net_allocNetbuff(child, size);
}
NET_RESERVE(ptp, net, buff, size) {
	struct net *child = net_getChild(net);
	return net_reserve(child, buff, size);
}
NET_SET_ALIGNMENT(ptp, net, align) {
	struct net *child = net_getChild(net);
	return net_setAlignment(child, align);
}
NET_GET_PAYLOAD(ptp, net, buff) {
	struct net *child = net_getChild(net);
	return net_getPayload(child, buff);
}
NET_SET_SIZE(ptp, net, buff, size) {
	struct net *child = net_getChild(net);
	return net_setSize(child, buff, size);
}
NET_GET_SIZE(ptp, net, buff) {
	struct net *child = net_getChild(net);
	return net_getSize(child, buff);
}
NET_FREE_BUFF(ptp, net, buff) {
	struct net *child = net_getChild(net);
	return net_freeNetbuff(child, buff);
}
NET_SET_TIMESTAMP(ptp, net, buff, timestamp) {
	struct net *child = net_getChild(net);
	return net_setTimestamp(child, buff, timestamp);
}
NET_GET_TIMESTAMP(ptp, net, buff, timestamp)  {
	struct net *child = net_getChild(net);
	return net_getTimestamp(child, buff, timestamp);
}
NET_RECV(ptp, n, buff) {
	struct net *child = net_getChild(n);
	struct net_ptp *net = (struct net_ptp *) n;
	/* in Disable state net is in forwoding mode */
	if (net->state != DISABLED) {
		uint8_t *payload = net_getPayload(n, buff);
		struct ptp_header *ptp;
		//01-1B-19-00-00-00
		if (payload[0] == 0x1 && payload[1] == 0x1B && payload[2] == 0x19 && payload[3] == 0x0 && payload[4] == 0x0 && payload[5] == 0x0) {
			uint16_t *type = (uint16_t *) (payload + 12);
			/* VLAN Header */
			if (*type == cpu_to_be16(VLAN_ID)) {
				type += VLAN_PACKET_LEN;
			}
			if (*type == cpu_to_be16(PTP_ID)) {
				ptp = (struct ptp_header *) (type + PTP_ID_LEN);
				if (UINT4_T_GET_VALUE(ptp->versionPTP) == 0x2) {
					switch(UINT4_T_GET_VALUE(ptp->messageType)) {
						case PTP_MESSAGE_TYPE_SYNC:
							handle_sync(net, ptp);
							break;
						case PTP_MESSAGE_TYPE_FOLLOW_UP:
							handle_followUp(net, ptp);
							break;
						case PTP_MESSAGE_TYPE_DELAY_REQ:
						case PTP_MESSAGE_TYPE_PDELAY_REQ:
						case PTP_MESSAGE_TYPE_PDELAY_RESP:
						case PTP_MESSAGE_TYPE_DELAY_RESP:
						case PTP_MESSAGE_TYPE_PDELAY_RESP_FOLLOW_UP:
						case PTP_MESSAGE_TYPE_ANNOUNCE:
							handle_announce(net, ptp);
							break;
						case PTP_MESSAGE_TYPE_SIGNALING:
						case PTP_MESSAGE_TYPE_MANAGEMENT:
							PRINTF("Ignore Messge not supported with type: 0x%x", UINT4_T_GET_VALUE(ptp->messageType));
							break;
					}
				} else {
					PRINTF("Not supported PTP Frame Version: 0x%x\n", UINT4_T_GET_VALUE(ptp->versionPTP));
				}
			} else {
				PRINTF("Not a PTP Frame type: 0x%x\n", *type);
			}
		}
		/* free Message */
		net_freeNetbuff(n, buff);
		return 0;
	}
	return net_recv(child, buff);
}
NET_SEND(ptp, net, buff) {
	struct net *parent = net_getParent(net);
	struct mac *mac = net_getMac(net);
	/* prefere mac */
	if (mac) {
		return mac_send(mac, buff);
	} else if (parent) {
		return net_send(parent, buff);
	} else {
		return -1;
	}
}
NET_GET_MTU(ptp, net) {
	struct net *child = net_getChild(net);
	return net_getMTU(child);
}
NET_SET_UP(ptp, net) {
	struct net *child = net_getChild(net);
	start((struct net_ptp *) net);
	return net_setUp(child);
}
NET_SET_DOWN(ptp, net) {
	struct net *child = net_getChild(net);
	stop((struct net_ptp *) net);
	return net_setDown(child);
}
NET_OPS(ptp);
