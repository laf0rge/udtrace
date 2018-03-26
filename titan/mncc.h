/* This file contains sections copied from
 * libosmocore/include/osmocom/gsm/protocol/gsm_04_08.h,
 * libosmocore/include/osmocom/gsm/mncc.h and
 * openbsc/include/openbsc/mncc.h
 */

#include <stdint.h>

/* GSM 04.08 Bearer Capability: Rate Adaption */
enum gsm48_bcap_ra {
	GSM48_BCAP_RA_NONE	= 0,
	GSM48_BCAP_RA_V110_X30	= 1,
	GSM48_BCAP_RA_X31	= 2,
	GSM48_BCAP_RA_OTHER	= 3,
};

/* GSM 04.08 Bearer Capability: Signalling access protocol */
enum gsm48_bcap_sig_access {
	GSM48_BCAP_SA_I440_I450	= 1,
	GSM48_BCAP_SA_X21	= 2,
	GSM48_BCAP_SA_X28_DP_IN	= 3,
	GSM48_BCAP_SA_X28_DP_UN	= 4,
	GSM48_BCAP_SA_X28_NDP	= 5,
	GSM48_BCAP_SA_X32	= 6,
};

/* GSM 04.08 Bearer Capability: User Rate */
enum gsm48_bcap_user_rate {
	GSM48_BCAP_UR_300	= 1,
	GSM48_BCAP_UR_1200	= 2,
	GSM48_BCAP_UR_2400	= 3,
	GSM48_BCAP_UR_4800	= 4,
	GSM48_BCAP_UR_9600	= 5,
	GSM48_BCAP_UR_12000	= 6,
	GSM48_BCAP_UR_1200_75	= 7,
};

/* GSM 04.08 Bearer Capability: Parity */
enum gsm48_bcap_parity {
	GSM48_BCAP_PAR_ODD	= 0,
	GSM48_BCAP_PAR_EVEN	= 2,
	GSM48_BCAP_PAR_NONE	= 3,
	GSM48_BCAP_PAR_ZERO	= 4,
	GSM48_BCAP_PAR_ONE	= 5,
};

/* GSM 04.08 Bearer Capability: Intermediate Rate */
enum gsm48_bcap_interm_rate {
	GSM48_BCAP_IR_8k	= 2,
	GSM48_BCAP_IR_16k	= 3,
};

/* GSM 04.08 Bearer Capability: Transparency */
enum gsm48_bcap_transp {
	GSM48_BCAP_TR_TRANSP	= 0,
	GSM48_BCAP_TR_RLP	= 1,
	GSM48_BCAP_TR_TR_PREF	= 2,
	GSM48_BCAP_TR_RLP_PREF	= 3,
};

/* GSM 04.08 Bearer Capability: Modem Type */
enum gsm48_bcap_modem_type {
	GSM48_BCAP_MT_NONE	= 0,
	GSM48_BCAP_MT_V21	= 1,
	GSM48_BCAP_MT_V22	= 2,
	GSM48_BCAP_MT_V22bis	= 3,
	GSM48_BCAP_MT_V23	= 4,
	GSM48_BCAP_MT_V26ter	= 5,
	GSM48_BCAP_MT_V32	= 6,
	GSM48_BCAP_MT_UNDEF	= 7,
	GSM48_BCAP_MT_AUTO_1	= 8,
};



#define GSM_MAX_FACILITY       128
#define GSM_MAX_SSVERSION      128
#define GSM_MAX_USERUSER       128

/* Expanded fields from GSM TS 04.08, Table 10.5.102 */
struct gsm_mncc_bearer_cap {
	int		transfer;	/* Information Transfer Capability */
	int 		mode;		/* Transfer Mode */
	int		coding;		/* Coding Standard */
	int		radio;		/* Radio Channel Requirement */
	int		speech_ctm;	/* CTM text telephony indication */
	int		speech_ver[8];	/* Speech version indication */
	struct {
		enum gsm48_bcap_ra		rate_adaption;
		enum gsm48_bcap_sig_access	sig_access;
		int				async;
		int				nr_stop_bits;
		int				nr_data_bits;
		enum gsm48_bcap_user_rate	user_rate;
		enum gsm48_bcap_parity		parity;
		enum gsm48_bcap_interm_rate	interm_rate;
		enum gsm48_bcap_transp		transp;
		enum gsm48_bcap_modem_type	modem_type;
	} data;
};

struct gsm_mncc_number {
	int 		type;
	int 		plan;
	int		present;
	int		screen;
	char		number[33];
};

struct gsm_mncc_cause {
	int		location;
	int		coding;
	int		rec;
	int		rec_val;
	int		value;
	int		diag_len;
	char		diag[32];
};

struct gsm_mncc_useruser {
	int		proto;
	char		info[GSM_MAX_USERUSER + 1]; /* + termination char */
};

struct gsm_mncc_progress {
	int		coding;
	int		location;
	int 		descr;
};

struct gsm_mncc_facility {
	int		len;
	char		info[GSM_MAX_FACILITY];
};

struct gsm_mncc_ssversion {
	int		len;
	char		info[GSM_MAX_SSVERSION];
};

struct gsm_mncc_cccap {
	int		dtmf;
	int		pcp;
};

enum {
	GSM_MNCC_BCAP_SPEECH	= 0,
	GSM_MNCC_BCAP_UNR_DIG	= 1,
	GSM_MNCC_BCAP_AUDIO	= 2,
	GSM_MNCC_BCAP_FAX_G3	= 3,
	GSM_MNCC_BCAP_OTHER_ITC = 5,
	GSM_MNCC_BCAP_RESERVED	= 7,
};


#define MNCC_SETUP_REQ		0x0101
#define MNCC_SETUP_IND		0x0102
#define MNCC_SETUP_RSP		0x0103
#define MNCC_SETUP_CNF		0x0104
#define MNCC_SETUP_COMPL_REQ	0x0105
#define MNCC_SETUP_COMPL_IND	0x0106
/* MNCC_REJ_* is perfomed via MNCC_REL_* */
#define MNCC_CALL_CONF_IND	0x0107
#define MNCC_CALL_PROC_REQ	0x0108
#define MNCC_PROGRESS_REQ	0x0109
#define MNCC_ALERT_REQ		0x010a
#define MNCC_ALERT_IND		0x010b
#define MNCC_NOTIFY_REQ		0x010c
#define MNCC_NOTIFY_IND		0x010d
#define MNCC_DISC_REQ		0x010e
#define MNCC_DISC_IND		0x010f
#define MNCC_REL_REQ		0x0110
#define MNCC_REL_IND		0x0111
#define MNCC_REL_CNF		0x0112
#define MNCC_FACILITY_REQ	0x0113
#define MNCC_FACILITY_IND	0x0114
#define MNCC_START_DTMF_IND	0x0115
#define MNCC_START_DTMF_RSP	0x0116
#define MNCC_START_DTMF_REJ	0x0117
#define MNCC_STOP_DTMF_IND	0x0118
#define MNCC_STOP_DTMF_RSP	0x0119
#define MNCC_MODIFY_REQ		0x011a
#define MNCC_MODIFY_IND		0x011b
#define MNCC_MODIFY_RSP		0x011c
#define MNCC_MODIFY_CNF		0x011d
#define MNCC_MODIFY_REJ		0x011e
#define MNCC_HOLD_IND		0x011f
#define MNCC_HOLD_CNF		0x0120
#define MNCC_HOLD_REJ		0x0121
#define MNCC_RETRIEVE_IND	0x0122
#define MNCC_RETRIEVE_CNF	0x0123
#define MNCC_RETRIEVE_REJ	0x0124
#define MNCC_USERINFO_REQ	0x0125
#define MNCC_USERINFO_IND	0x0126
#define MNCC_REJ_REQ		0x0127
#define MNCC_REJ_IND		0x0128

#define MNCC_BRIDGE		0x0200
#define MNCC_FRAME_RECV		0x0201
#define MNCC_FRAME_DROP		0x0202
#define MNCC_LCHAN_MODIFY	0x0203
#define MNCC_RTP_CREATE		0x0204
#define MNCC_RTP_CONNECT	0x0205
#define MNCC_RTP_FREE		0x0206

#define GSM_TCHF_FRAME		0x0300
#define GSM_TCHF_FRAME_EFR	0x0301
#define GSM_TCHH_FRAME		0x0302
#define GSM_TCH_FRAME_AMR	0x0303
#define GSM_BAD_FRAME		0x03ff

#define MNCC_SOCKET_HELLO	0x0400

#define GSM_MAX_FACILITY	128
#define GSM_MAX_SSVERSION	128
#define GSM_MAX_USERUSER	128

#define	MNCC_F_BEARER_CAP	0x0001
#define MNCC_F_CALLED		0x0002
#define MNCC_F_CALLING		0x0004
#define MNCC_F_REDIRECTING	0x0008
#define MNCC_F_CONNECTED	0x0010
#define MNCC_F_CAUSE		0x0020
#define MNCC_F_USERUSER		0x0040
#define MNCC_F_PROGRESS		0x0080
#define MNCC_F_EMERGENCY	0x0100
#define MNCC_F_FACILITY		0x0200
#define MNCC_F_SSVERSION	0x0400
#define MNCC_F_CCCAP		0x0800
#define MNCC_F_KEYPAD		0x1000
#define MNCC_F_SIGNAL		0x2000

struct gsm_mncc {
	/* context based information */
	uint32_t	msg_type;
	uint32_t	callref;

	/* which fields are present */
	uint32_t	fields;

	/* data derived informations (MNCC_F_ based) */
	struct gsm_mncc_bearer_cap	bearer_cap;
	struct gsm_mncc_number		called;
	struct gsm_mncc_number		calling;
	struct gsm_mncc_number		redirecting;
	struct gsm_mncc_number		connected;
	struct gsm_mncc_cause		cause;
	struct gsm_mncc_progress	progress;
	struct gsm_mncc_useruser	useruser;
	struct gsm_mncc_facility	facility;
	struct gsm_mncc_cccap		cccap;
	struct gsm_mncc_ssversion	ssversion;
	struct	{
		int		sup;
		int		inv;
	} clir;
	int		signal;

	/* data derived information, not MNCC_F based */
	int		keypad;
	int		more;
	int		notify; /* 0..127 */
	int		emergency;
	char		imsi[16];

	unsigned char	lchan_type;
	unsigned char	lchan_mode;
};

struct gsm_data_frame {
	uint32_t	msg_type;
	uint32_t	callref;
	unsigned char	data[0];
};

#define MNCC_SOCK_VERSION	5
struct gsm_mncc_hello {
	uint32_t	msg_type;
	uint32_t	version;

	/* send the sizes of the structs */
	uint32_t	mncc_size;
	uint32_t	data_frame_size;

	/* send some offsets */
	uint32_t	called_offset;
	uint32_t	signal_offset;
	uint32_t	emergency_offset;
	uint32_t	lchan_type_offset;
};

struct gsm_mncc_rtp {
	uint32_t	msg_type;
	uint32_t	callref;
	uint32_t	ip;
	uint16_t	port;
	uint32_t	payload_type;
	uint32_t	payload_msg_type;
};

struct gsm_mncc_bridge {
	uint32_t	msg_type;
	uint32_t	callref[2];
};
