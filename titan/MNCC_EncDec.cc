#include "mncc.h"
#include "MNCC_Types.hh"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

namespace MNCC__Types {

static void enc_bcap(struct gsm_mncc_bearer_cap *out, const MNCC__bearer__cap& in)
{
	out->transfer = in.transfer();
	out->mode = in.mode();
	out->coding = in.coding();
	out->radio = in.radio();
	out->speech_ctm = in.speech__ctm();

	for (int i = 0; i < in.speech__ver().lengthof(); i++)
		out->speech_ver[i] = in.speech__ver()[i];

	if (in.data().is_value()) {
		MNCC__bearer__cap__data data = in.data();
		out->data.rate_adaption = (gsm48_bcap_ra) (int) data.rate__adaptation();
		out->data.sig_access = (gsm48_bcap_sig_access) (int) data.sig__access();
		out->data.async = data.async();
		out->data.nr_stop_bits = data.nr__stop__bits();
		out->data.nr_data_bits = data.nr__data__bits();
		out->data.user_rate = (gsm48_bcap_user_rate) (int) data.user__rate();
		out->data.parity = (gsm48_bcap_parity) (int) data.parity();
		out->data.interm_rate = (gsm48_bcap_interm_rate) (int) data.interm__rate();
		out->data.transp = (gsm48_bcap_transp) (int) data.transp();
		out->data.modem_type = (gsm48_bcap_modem_type) (int) data.modem__type();
	}

}

static MNCC__bearer__cap dec_bcap(const struct gsm_mncc_bearer_cap *in)
{
	MNCC__bearer__cap__data data;
	MNCC__speech__vers vers;
	data = MNCC__bearer__cap__data((GSM48__bcap__ra) in->data.rate_adaption,
					(GSM48__bcap__sig__access) in->data.sig_access,
					in->data.async,
					in->data.nr_stop_bits,
					in->data.nr_data_bits,
					(GSM48__bcap__user__rate) in->data.user_rate,
					(GSM48__bcap__parity) in->data.parity,
					(GSM48__bcap__interm__rate) in->data.interm_rate,
					(GSM48__bcap__transp) in->data.transp,
					(GSM48__bcap__modem__type) in->data.modem_type);

	for (unsigned int i = 0; i < ARRAY_SIZE(in->speech_ver); i++)
		vers[0] = in->speech_ver[0];

	return MNCC__bearer__cap(in->transfer, in->mode, in->coding, in->radio, in->speech_ctm,
				 vers, data);
}


static void enc_number(struct gsm_mncc_number *num, const MNCC__number& in)
{
	num->type = in.number__type();
	num->plan = in.plan();
	num->present = in.presence();
	num->screen = in.screen();
	strncpy(num->number, in.number(), sizeof(num->number));
}

static MNCC__number dec_number(const struct gsm_mncc_number *num)
{
	return MNCC__number(num->type, num->plan,num->present, num->screen, num->number);
}

OCTETSTRING enc__MNCC__PDU(const MNCC__PDU& in)
{
	struct gsm_mncc mncc;
	OCTETSTRING ret_val;

	memset(&mncc, 0, sizeof(mncc));
	mncc.msg_type = in.msg__type();

	switch (in.u().get_selection()) {
	case MNCC__MsgUnion::ALT_signal: {
		const MNCC__PDU__Signal& in_sig = in.u().signal();
		mncc.callref = in_sig.callref();
		if (in_sig.bearer__cap().is_value()) {
			enc_bcap(&mncc.bearer_cap, in_sig.bearer__cap());
			mncc.fields |= MNCC_F_BEARER_CAP;
		}
		if (in_sig.called().is_value()) {
			enc_number(&mncc.called, in_sig.called());
			mncc.fields |= MNCC_F_CALLED;
		}
		if (in_sig.calling().is_value()) {
			enc_number(&mncc.calling, in_sig.calling());
			mncc.fields |= MNCC_F_CALLING;
		}
		if (in_sig.redirecting().is_value()) {
			enc_number(&mncc.redirecting, in_sig.redirecting());
			mncc.fields |= MNCC_F_REDIRECTING;
		}
		if (in_sig.connected().is_value()) {
			enc_number(&mncc.connected, in_sig.connected());
			mncc.fields |= MNCC_F_CONNECTED;
		}
		if (in_sig.cause().is_value()) {
			const MNCC__cause &cause = in_sig.cause();
			TTCN_Buffer ttcn_buffer(cause.diag());
			mncc.cause.location = cause.location();
			mncc.cause.coding = cause.coding();
			mncc.cause.rec = cause.rec();
			mncc.cause.rec_val = cause.rec__val();
			mncc.cause.value = cause.val();
			mncc.cause.diag_len = ttcn_buffer.get_len();
			if (mncc.cause.diag_len > (int) sizeof(mncc.cause.diag)) {
				TTCN_error("MNCC diagnostics length %u too long", mncc.cause.diag_len);
				mncc.cause.diag_len = sizeof(mncc.cause.diag);
			}
			memcpy(mncc.cause.diag, ttcn_buffer.get_data(), ttcn_buffer.get_len());
			mncc.fields |= MNCC_F_CAUSE;
		}
		if (in_sig.progress().is_value()) {
			const MNCC__progress &progress = in_sig.progress();
			mncc.progress.coding = progress.coding();
			mncc.progress.location = progress.location();
			mncc.progress.descr = progress.descr();
			mncc.fields |= MNCC_F_PROGRESS;
		}
		if (in_sig.useruser().is_value()) {
			const MNCC__useruser &useruser = in_sig.useruser();
			mncc.useruser.proto = useruser.proto();
			strncpy(mncc.useruser.info, useruser.info(), sizeof(mncc.useruser.info));
			mncc.fields |= MNCC_F_USERUSER;
		}
		if (in_sig.facility().is_value()) {
			const CHARSTRING &fac = in_sig.facility();
			strncpy(mncc.facility.info, fac, sizeof(mncc.facility.info));
			mncc.facility.len = strlen(mncc.facility.info);
			mncc.fields |= MNCC_F_FACILITY;
		}
		if (in_sig.cccap().is_value()) {
			const MNCC__cccap &cccap = in_sig.cccap();
			mncc.cccap.dtmf = cccap.dtmf();
			mncc.cccap.pcp = cccap.pcp();
			mncc.fields |= MNCC_F_CCCAP;
		}
		if (in_sig.ssversion().is_value()) {
			const CHARSTRING &ssv = in_sig.ssversion();
			strncpy(mncc.ssversion.info, ssv, sizeof(mncc.ssversion.info));
			mncc.ssversion.len = strlen(mncc.ssversion.info);
			mncc.fields |= MNCC_F_SSVERSION;
		}
		mncc.clir.sup = in_sig.clir__sup();
		mncc.clir.inv = in_sig.clir__inv();
		if (in_sig.signal().is_value()) {
			const INTEGER &sig = in_sig.signal();
			mncc.signal = sig;
			mncc.fields |= MNCC_F_SIGNAL;
		}
		if (in_sig.keypad().is_value()) {
			const CHARSTRING &kpd = in_sig.keypad();
			mncc.signal = (int) kpd[0].get_char();
			mncc.fields |= MNCC_F_KEYPAD;
		}
		mncc.more = in_sig.more();
		mncc.notify = in_sig.notify();
		if (in_sig.emergency().is_value()) {
			const INTEGER &emerg = in_sig.emergency();
			mncc.emergency = emerg;
			mncc.fields |= MNCC_F_EMERGENCY;
		}
		strncpy(mncc.imsi, in_sig.imsi(), sizeof(mncc.imsi));
		mncc.lchan_type = in_sig.lchan__type();
		mncc.lchan_mode = in_sig.lchan__mode();
		ret_val = OCTETSTRING(sizeof(mncc), (uint8_t *)&mncc);
		}
		break;
	case MNCC__MsgUnion::ALT_data:
		struct gsm_data_frame data;
		memset(&data, 0, sizeof(data));
		data.msg_type = in.msg__type();
		ret_val = OCTETSTRING(sizeof(data), (uint8_t *)&data);
		ret_val = ret_val & in.u().data().data();
		break;
	case MNCC__MsgUnion::ALT_rtp:
		struct gsm_mncc_rtp rtp;
		memset(&rtp, 0, sizeof(rtp));
		rtp.msg_type = in.msg__type();
		rtp.callref = in.u().rtp().callref();
		rtp.ip = in.u().rtp().ip();
		rtp.port = in.u().rtp().rtp__port();
		rtp.payload_type = in.u().rtp().payload__type();
		rtp.payload_msg_type = in.u().rtp().payload__msg__type();
		ret_val = OCTETSTRING(sizeof(rtp), (uint8_t *) &rtp);
		break;
	case MNCC__MsgUnion::ALT_hello:
		struct gsm_mncc_hello hello;
		memset(&hello, 0, sizeof(hello));
		hello.msg_type = in.msg__type();
		hello.version = in.u().hello().version();
		hello.mncc_size = in.u().hello().mncc__size();
		hello.data_frame_size = in.u().hello().data__frame__size();
		hello.called_offset = in.u().hello().called__offset();
		hello.signal_offset = in.u().hello().signal__offset();
		hello.emergency_offset = in.u().hello().emergency__offset();
		hello.lchan_type_offset = in.u().hello().lchan__type__offset();
		ret_val = OCTETSTRING(sizeof(hello), (uint8_t *) &hello);
		break;
	}

	return ret_val;
}

MNCC__PDU dec__MNCC__PDU(const OCTETSTRING& in)
{
	TTCN_Buffer ttcn_buffer(in);
	const struct gsm_mncc *in_mncc;
	MNCC__PDU__Signal sign;
	const struct gsm_mncc_hello *in_hello;
	MNCC__PDU__Hello hello;
	const struct gsm_data_frame *in_data;
	MNCC__PDU__Data data;
	const struct gsm_mncc_rtp *in_rtp;
	MNCC__PDU__Rtp rtp;
	MNCC__MsgUnion u;

	in_mncc = (struct gsm_mncc *) ttcn_buffer.get_read_data();

	sign.set_implicit_omit();
	hello.set_implicit_omit();
	data.set_implicit_omit();
	rtp.set_implicit_omit();

	switch (in_mncc->msg_type) {
	case MNCC_SOCKET_HELLO:
		in_hello = (const struct gsm_mncc_hello *) in_mncc;
		hello = MNCC__PDU__Hello(in_hello->version,
					 in_hello->mncc_size,
					 in_hello->data_frame_size,
					 in_hello->called_offset,
					 in_hello->signal_offset,
					 in_hello->emergency_offset,
					 in_hello->lchan_type_offset);
		u.hello() = hello;
		break;
	case GSM_TCHF_FRAME:
	case GSM_TCHF_FRAME_EFR:
	case GSM_TCHH_FRAME:
	case GSM_TCH_FRAME_AMR:
	case GSM_BAD_FRAME:
		in_data = (const struct gsm_data_frame *) in_mncc;
		u.data() = MNCC__PDU__Data(in_data->callref,
					   substr(in, offsetof(struct gsm_data_frame, data),
						  in.lengthof() - offsetof(struct gsm_data_frame, data)));
		break;
	case MNCC_RTP_CREATE:
	case MNCC_RTP_CONNECT:
	case MNCC_RTP_FREE:
		in_rtp = (const struct gsm_mncc_rtp *) in_mncc;
		rtp = MNCC__PDU__Rtp(in_rtp->callref, in_rtp->ip, in_rtp->port, in_rtp->payload_type,
				     in_rtp->payload_msg_type);
		u.rtp() = rtp;
		break;
	default:
		sign.callref() = in_mncc->callref;
		if (in_mncc->fields & MNCC_F_BEARER_CAP) {
			sign.bearer__cap() = dec_bcap(&in_mncc->bearer_cap);
		}
		if (in_mncc->fields & MNCC_F_CALLED)
			sign.called() = dec_number(&in_mncc->called);
		if (in_mncc->fields & MNCC_F_CALLING)
			sign.calling() = dec_number(&in_mncc->calling);
		if (in_mncc->fields & MNCC_F_REDIRECTING)
			sign.redirecting() = dec_number(&in_mncc->redirecting);
		if (in_mncc->fields & MNCC_F_CONNECTED)
			sign.connected() = dec_number(&in_mncc->connected);
		if (in_mncc->fields & MNCC_F_CAUSE) {
			sign.cause() = MNCC__cause(in_mncc->cause.location,
						   in_mncc->cause.coding,
						   in_mncc->cause.rec,
						   in_mncc->cause.rec_val,
						   in_mncc->cause.value,
						   OCTETSTRING(in_mncc->cause.diag_len,
								(const uint8_t *)in_mncc->cause.diag));
		}
		if (in_mncc->fields & MNCC_F_USERUSER) {
			sign.useruser() = MNCC__useruser(in_mncc->useruser.proto,
							 CHARSTRING(in_mncc->useruser.info));
		}
		if (in_mncc->fields & MNCC_F_PROGRESS) {
			sign.progress() = MNCC__progress(in_mncc->progress.coding,
							 in_mncc->progress.location,
							 in_mncc->progress.descr);
		}
		if (in_mncc->fields & MNCC_F_EMERGENCY)
			sign.emergency() = in_mncc->emergency;
		if (in_mncc->fields & MNCC_F_FACILITY)
			sign.facility() = CHARSTRING(in_mncc->facility.info);
		if (in_mncc->fields & MNCC_F_SSVERSION)
			sign.ssversion() = CHARSTRING(in_mncc->ssversion.info);
		if (in_mncc->fields & MNCC_F_CCCAP)
			sign.cccap() = MNCC__cccap(in_mncc->cccap.dtmf, in_mncc->cccap.pcp);
		if (in_mncc->fields & MNCC_F_KEYPAD) {
			char kpd[2] = { (char) in_mncc->keypad, 0 };
			sign.keypad() = CHARSTRING(kpd);
		}
		if (in_mncc->fields & MNCC_F_SIGNAL)
			sign.signal() = in_mncc->signal;

		sign.clir__sup() = in_mncc->clir.sup;
		sign.clir__inv() = in_mncc->clir.inv;
		sign.more() = in_mncc->more;
		sign.notify() = in_mncc->notify;
		sign.imsi() = CHARSTRING(in_mncc->imsi);
		sign.lchan__type() = in_mncc->lchan_type;
		sign.lchan__mode() = in_mncc->lchan_mode;
		u.signal() = sign;
		break;
	}
	return MNCC__PDU(in_mncc->msg_type, u);
}

}
