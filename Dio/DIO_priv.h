#ifndef DIO_priv
#define DIO_priv

#define conc_helper(A0,A1,A2,A3,A4,A5,A6,A7) 0b##A7##A6##A5##A4##A3##A2##A1##A0
#define conc(PA7,PA6,PA5,PA4,PA3,PA2,PA1,PA0) conc_helper(PA7,PA6,PA5,PA4,PA3,PA2,PA1,PA0)

#endif
