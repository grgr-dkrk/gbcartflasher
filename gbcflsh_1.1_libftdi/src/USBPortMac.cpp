/*****************************************************************************
** USBPortMac.cpp
** Author: Kraku
*****************************************************************************/
#include "const.h"
#include "USBPortMac.h"
#include "Settings.h"
#include <ftdi.h>
#include <iostream>

extern "C" {
  #include <time.h>
  #include <stdio.h>
  #include <stdlib.h>
}

USBPortMac::USBPortMac():AbstractPort () {
  ftdi_init(&ftdic);
}

bool USBPortMac::open_port (unsigned int port_num) {
  int ftStatus = ftdi_usb_open_desc_index(&ftdic, 0x0403, 0x6001, "FT232R USB UART", NULL, port_num);
  if (ftStatus < 0) {
    printf("couldn't open port : %d\n",ftStatus);
    return FALSE;
  }

  /* choose speed */
  if (Settings::speed == STANDARD) {
    if (ftdi_set_baudrate (&ftdic, 185000) < 0)
      return FALSE;
  } else if (Settings::speed == LOW) {
    if (ftdi_set_baudrate (&ftdic, 125000) < 0)
      return FALSE;
  } else if (Settings::speed == HIGH) {
    if (ftdi_set_baudrate (&ftdic, 375000) < 0)
      return FALSE;
  }

  if (ftdi_set_latency_timer (&ftdic, 2) < 0)
    return FALSE;
  if (ftdi_set_line_property (&ftdic, BITS_8, STOP_BIT_1, NONE) < 0)
    return FALSE;
  //if(FT_SetTimeouts(ftHandle,5000,0) != FT_OK)
  //      return FALSE;
  //if(ftdi_enable_bitbang(&ftdic,0xFF) < 0)
  //      return FALSE;

  return TRUE;			/* all ok */
}

bool USBPortMac::close_port () {
  ftdi_usb_close (&ftdic);
  return TRUE;
}

int USBPortMac::send_packet (unsigned char packet[PACKETSIZE]) {
  int bytesSent;
  bytesSent = ftdi_write_data (&ftdic, packet, PACKETSIZE);
  return bytesSent;
}

bool USBPortMac::send_char (unsigned char character) {
  int bytesSent;
  bytesSent = ftdi_write_data (&ftdic, &character, 1);
  return bytesSent == 1;
}

int USBPortMac::receive_char (void) {
  time_t tp = time (NULL);
  unsigned char character;
  int bytesReceived = 0;
  do {
    bytesReceived = ftdi_read_data (&ftdic, &character, 1);
    if (bytesReceived != 0)
      break;
  } while (time (NULL) - tp < SLEEPTIME);

  if (bytesReceived == 0)
    return TIMEOUT;
  if (character == ACK || character == END)
    return character;
  else
    return NAK;
}

int USBPortMac::receive_packet (unsigned char *packet) {
  time_t tp = time (NULL);
  int bytesReceived = 0, bytesLeft;
  do {
    bytesReceived = ftdi_read_data (&ftdic, packet, 1);
  } while (time (NULL) - tp < SLEEPTIME && bytesReceived == 0);
  if (bytesReceived == 0)
    return TIMEOUT;
  else {
    if (packet[0] != DATA) {
      if (packet[0] == ACK)
        return ACK;
      else if (packet[0] == END)
        return END;
      else
        return NAK;
    } else {
      unsigned int remaining = PACKETSIZE - 1;
      tp = time (NULL);
      bytesReceived = 0;
      do {
        bytesLeft = remaining;

        bytesReceived =
        ftdi_read_data (&ftdic, &packet[PACKETSIZE - remaining], bytesLeft);
        remaining -= bytesReceived;
        tp = time (NULL);
      } while (time (NULL) - tp < SLEEPTIME && remaining != 0);
      if (remaining > 0)
        return TIMEOUT;
      else
        return DATA;
    }
  }
}
