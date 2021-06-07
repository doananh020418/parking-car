void UART_Init(){
    // Baud rate configuration
    BRGH = 1;
    SPBRG = 25;
    // Enable Asynchronous Serial Port
    SYNC = 0;
    SPEN = 1;
    // Configure Rx-Tx pin for UART 
    TRISC6 = 1;
    TRISC7 = 1;
    // Configure interrupt
    RCIE = 1;
    PEIE = 1;
    //
    CREN = 1; 
}
    

uint8_t UART_Receive(){
    while(RCIF==0);    // Wait till the data is received 
    RCIF=0;            // Clear receiver flag
    return(RCREG);     // Return the received data to calling function
}

