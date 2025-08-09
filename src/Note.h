
// void IND_OutputFormat(uint16_t value, uint8_t comma, uint8_t position, uint8_t amount)
// {
//   uint8_t tmp;
//   uint8_t i;

//   if ((position <= IND_AMOUNT_NUM)&&(position != 0)){
// 	  uint8_t tcifr=amount+position;
//      for(i = 0; i < amount; i++){
//        tmp = value % 10;
//         //if ((i == 0)||(value > 0)||(position <= comma)){
// 		if ((value > 0)||(position <= comma)){ 
//            ind_buf[tcifr-2] = read_byte_flash(number[tmp]);
//         }
//         else{
//            ind_buf[tcifr-2] = read_byte_flash(number[IND_EMPTY_NUM]);
//         }      
//         value = value/10;
// 		  tcifr--;
//         position++;
//         if (position > IND_AMOUNT_NUM){
//            break;
//         } 
//      }
//   }
// /********************************************************************************/
     
//   if ((comma <= IND_AMOUNT_NUM) && (comma != 0)) {
//     ind_buf[comma-1] |= 1<<IND_COM;
//   }
// }

// /******************************************************************************/
// void IND_Update(void)
// {
//   static uint8_t count = 0;     
//   uint8_t i;
  
//   /*________________ подключение по SPI____________________________*/
// 	uint8_t tmp;
// 	#define IND_OutSeg(port, value) port |= (value)
//   #define IND_LightOutSeg(port, value) port &= ~(value)
	
// 	#define IND_OutDig(port, value) port &= ~(value)
//   #define IND_LightOutDig(port, value) port |= (value)

//   /*гасим индикатор*/
//   IND_LightOutSeg(spi_buf[IND_SHIFT_REG_SEG],IND_LIGHT_OUT);

     
//   tmp = 0;
//   for (i = 0; i < IND_AMOUNT_NUM; i++){
// 	 //for (i = IND_AMOUNT_NUM; i > 0; i--){ 
//      tmp |= (1<<read_byte_flash(digit[i]));  
//   }
//   IND_LightOutDig(spi_buf[IND_SHIFT_REG_DIG1],((uint8_t)tmp));
  
  
//   //SPI_WriteArray(IND_AMOUNT_REG, spi_buf);
// 	//SPI_P_WriteArray(IND_AMOUNT_REG, spi_buf);

//   /*___________реализация поразрядной развертки___________*/ 
  
//   tmp = 1<<read_byte_flash(digit[count]);
//   IND_OutDig(spi_buf[IND_SHIFT_REG_DIG1], ((uint8_t)tmp));  
  
//   IND_OutSeg(spi_buf[IND_SHIFT_REG_SEG], ind_buf[count]);
  
//   count++;
//   if (count >= IND_AMOUNT_NUM){
//      count = 0;  
//   }

//   //SPI_WriteArray(IND_AMOUNT_REG, spi_buf);
// 	SPI_P_WriteArray(IND_AMOUNT_REG, spi_buf); 
// }
