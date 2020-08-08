#ifndef std_functions
#define std_functions

#define ref_low 0x0f
#define ref_high 0xf0
#define shift_ref 1
#define nibble_ref 4
#define port_ref 0xff
#define zero_ref 0x00
#define port_ShftRef 8

#define assignbit(port,bit,value) do{ \
										if(!(value)) \
										{ \
											clrbit(port,bit);\
										} \
										else \
										{ \
											setbit(port,bit); \
										} \
									}while(0)
#define setbit(port,bit)         port = (port |  shift_ref << (bit))

#define clrbit(port,bit)         port &= ~(shift_ref << (bit))

#define togglebit(port,bit)      port = port ^ (shift_ref << (bit))

#define getbit(port,bit)         ((port >> bit) & shift_ref)

#define setnibblelow(port)       port =  (port | ref_low)

#define setnibblehigh(port)      port =  (port | ref_high)

#define clrnibblelow(port)       port = (port & ref_high)

#define clrnibblehigh(port)      port = (port & ref_low)

#define togglenibblelow(port)    port = port ^ ~(ref_low << nibble_ref)

#define togglenibblehigh(port)   port = port ^ ~(ref_high >> nibble_ref)

#define assignnibblelow(port,value)    port =  (((port >> nibble_ref) << nibble_ref) | (value))
										

#define assignnibblehigh(port,value)   port =  (((port << nibble_ref) >> nibble_ref) | (value << nibble_ref))
										 

#define getnibblelow(port)             ((port << nibble_ref) >> nibble_ref) & ref_low

#define getnibblehigh(port)            ((port >> nibble_ref) << nibble_ref) & ref_high
										   
#define setport(port)                  port = port | port_ref
											
#define clrport(port)                  port = port & zero_ref
											
#define getport(port)                  port | port_ref
											
#define toggleport(port)               port = ~(port)
	
#define assignport(port,value)         do{clrport(port);\
										port = (port | value) ;\
										}while(0)
										
												
#endif
