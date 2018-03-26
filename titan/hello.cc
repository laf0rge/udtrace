#include <errno.h>

extern char *program_invocation_short_name;

#include <iostream>
#include "MNCC_Types.hh"
#include "PCUIF_Types.hh"

using namespace std;

extern "C" {

void pcu_dissector(int fd, bool is_out, const char *fn, const uint8_t *data, unsigned int len)
{
	OCTETSTRING oct(len, data);
	PCUIF__Types::PCUIF__Message pdu = PCUIF__Types::dec__PCUIF__Message(oct);
	TTCN_Logger::begin_event(TTCN_ERROR);
	TTCN_Logger::log_event("%s(%d) %s: ", fn, fd, is_out ? "Tx" : "Rx");
	pdu.log();
	TTCN_Logger::end_event();
}


void mncc_dissector(int fd, bool is_out, const char *fn, const uint8_t *data, unsigned int len)
{
	OCTETSTRING oct(len, data);
	MNCC__Types::MNCC__PDU pdu = MNCC__Types::dec__MNCC__PDU(oct);
	TTCN_Logger::begin_event(TTCN_ERROR);
	TTCN_Logger::log_event("%s(%d) %s: ", fn, fd, is_out ? "Tx" : "Rx");
	pdu.log();
	TTCN_Logger::end_event();
}

__attribute__ ((constructor)) static void init_mncc(void)
{
	TTCN_Runtime::set_state(TTCN_Runtime::SINGLE_CONTROLPART);
	//TTCN_Runtime::install_signal_handlers();
	TTCN_Logger::initialize_logger();
	TTCN_Logger::set_executable_name(program_invocation_short_name);
	TTCN_Logger::set_start_time();
	TTCN_Logger::open_file();
}

}
