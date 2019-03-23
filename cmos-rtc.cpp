/*
 * CMOS Real-time Clock
 * SKELETON IMPLEMENTATION -- TO BE FILLED IN FOR TASK (3)
 */

/*
 * STUDENT NUMBER: s
 */
#include <infos/drivers/timer/rtc.h>
#include <infos/mm/mm.h>
#include <infos/kernel/sched.h>
#include <infos/util/lock.h>
#include <arch/x86/pio.h>
using namespace infos::kernel;
using namespace infos::util;
using namespace infos::arch::x86;
using namespace infos::drivers;
using namespace infos::drivers::timer;
using namespace infos::mm;

class CMOSRTC : public RTC {
public:
	static const DeviceClass CMOSRTCDeviceClass;

	
	const DeviceClass& device_class() const override
	{
		return CMOSRTCDeviceClass;
	}

	/**
	 * Interrogates the RTC to read the current date & time.
	 * @param tp Populates the tp structure with the current data & time, as
	 * given by the CMOS RTC device.
	 */
	void read_timepoint(RTCTimePoint& tp) override
	{
		
		UniqueIRQLock l;
		// FILL IN THIS METHOD - WRITE HELPER METHODS IF NECESSARY
		while(read_register_a()){
			// do nothing, until the update process finished
		}
		uint8_t twentyfour_hours_format = check_format();
		uint8_t binary_mode_enabled = check_mode();
		
		mm_log.messagef(LogLevel::DEBUG, "twentyfour_hours_format is: %d:",twentyfour_hours_format);
		mm_log.messagef(LogLevel::DEBUG, "binary_mode_enabled is :%d",binary_mode_enabled);

		unsigned short temp_seconds = read_registers(0x00);
		unsigned short temp_minutes = read_registers(0x02);
		unsigned short temp_hours = read_registers(0x04);
		unsigned short temp_day_of_month = read_registers(0x07);
		unsigned short temp_month = read_registers(0x08);
		unsigned short temp_year = read_registers(0x09);

		

		uint8_t isPM = (!twentyfour_hours_format) && (temp_hours & 0x80);
		temp_hours = temp_hours & 0x7F; 



		if(! binary_mode_enabled ){
			temp_seconds = conversion_mode(temp_seconds);
			temp_minutes = conversion_mode(temp_minutes);
			temp_hours = conversion_mode(temp_hours);
			temp_day_of_month = conversion_mode(temp_day_of_month);
			temp_month = conversion_mode(temp_month);
			temp_year = conversion_mode(temp_year);
		}
		
		if(isPM){
			temp_hours = (temp_hours + 12) % 24;
		}

		mm_log.messagef(LogLevel::DEBUG, "temp_seconds is: %d:",temp_seconds);
		mm_log.messagef(LogLevel::DEBUG, "temp_minutes is :%d",temp_minutes);
		mm_log.messagef(LogLevel::DEBUG, "temp_hours is :%d",temp_hours);
		mm_log.messagef(LogLevel::DEBUG, "temp_day_of_month is: %d",temp_day_of_month);
		mm_log.messagef(LogLevel::DEBUG, "temp_month is :%d",temp_month);
		mm_log.messagef(LogLevel::DEBUG, "temp_year is :%d",temp_year);

		tp.seconds = temp_seconds;
		tp.minutes = temp_minutes;
		tp.hours = temp_hours;
		tp.day_of_month = temp_day_of_month;
		tp.month = temp_month;
		tp.year = temp_year;

		mm_log.messagef(LogLevel::DEBUG, "temp_seconds is: %d:",tp.seconds);
		mm_log.messagef(LogLevel::DEBUG, "temp_minutes is :%d",tp.minutes);
		mm_log.messagef(LogLevel::DEBUG, "temp_hours is :%d",tp.hours);
		mm_log.messagef(LogLevel::DEBUG, "temp_day_of_month is: %d",tp.day_of_month);
		mm_log.messagef(LogLevel::DEBUG, "temp_month is :%d",tp.month);
		mm_log.messagef(LogLevel::DEBUG, "temp_year is :%d",tp.year);
		return;
	}

	unsigned short conversion_mode(unsigned short argument){
		argument = (argument & 0x0F) + ((argument / 16) * 10);
		return argument;
	}
	
	unsigned short read_registers(uint8_t offset){
		__outb(0x70,offset);
		uint8_t value = __inb(0x71);
		return value;
	}               

	bool read_register_a(){
		// activate offset 0x0A, which is place register A lives
		__outb(0x70,0x0A);
		//read data
		uint8_t v = __inb(0x71);
		//extract the 7 bit
		uint8_t flag = v & 0x80;
		return flag;
	}

	// this is to check if RTC is in Binary or BCD mode
	// This function returns true if RTC is in binary mode 
	// else returns false
	int check_mode(){
		// activate offset 0x0B, which is place register B lives
		__outb(0x70,0x0B);
		//read data
		uint8_t value = __inb(0x71);
		// bit 2 is set
		if( value & 0x04 ){
			return 1;
		}else{
			return 0;
		}
	}

	// this is to check if RTC time is in 12 or 24 hours format
	// This function returns true if RTC is in binary mode 
	// else returns false
	uint8_t check_format(){
		// activate offset 0x0A, which is place register A lives
		__outb(0x70,0x0B);
		//read data
		uint8_t value = __inb(0x71);
		//if 24 hour mode is not set
		if( !(value & 0x02) ){
			return 0;
		}else{
			return 1;
		}
	}


};

const DeviceClass CMOSRTC::CMOSRTCDeviceClass(RTC::RTCDeviceClass, "cmos-rtc");

RegisterDevice(CMOSRTC);
