#ifndef COMMON_H
#define COMMON_H
#include "EEPROM.h"
  extern unsigned eep_read( unsigned, void*, uint16_t );
  extern unsigned eep_write( unsigned, void*, uint16_t );
  bool eep_empty( void*, uint16_t );
  template<typename D1, typename D2> void load_param( unsigned& addr, D1& data, D2 def, bool loaddef = false ){
    size_t size = sizeof( data );
    uint16_t _addr = eep_read( addr, &data, size );
  
      if( eep_empty( &data, size ) || loaddef ){
        memcpy( &data, &def, size );
        eep_write( addr, &data, size );
      }
  
    addr = _addr;
  } 
  template<typename D> void save_param( unsigned& addr, D data ){
    size_t size = sizeof( data );
    eep_write( addr, &data, size );
    addr = addr+size;
  }
#endif