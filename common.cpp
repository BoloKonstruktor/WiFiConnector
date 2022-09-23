#include "common.h"
  unsigned eep_read( unsigned offset, void* data, uint16_t size ){
  uint8_t* buff = (uint8_t*)data;
  
      for ( uint16_t i = 0; i < size; i++ ){
#ifdef ESP32
        buff[i] = EEPROM.readUChar( offset );
#else
		buff[i] = EEPROM.read( offset );
#endif
        offset++;
  
          if ( offset == EEPROM.length() ) break;
      }
   
  return offset;
  }
  
  unsigned eep_write( unsigned offset, void* data, uint16_t size ){
    const uint8_t* buff = (uint8_t*)data;
      
      for ( uint16_t i = 0; i < size; i++ ){
#ifdef ESP32
        EEPROM.writeUChar( offset, buff[i] );
#else
		EEPROM.write( offset, buff[i] );
#endif
        offset++;
  
          if ( offset == EEPROM.length() ) break;
      }
  
    EEPROM.commit();
    return offset+size;
  }
  
  bool eep_empty( void* data, uint16_t size ){
    const uint8_t* buff = (uint8_t*)data;
  
      for( uint16_t i = 0; i < size; i++ ){
            
        if( buff[i] != 0xFF ) return false;
      }
  
    return true;
  }