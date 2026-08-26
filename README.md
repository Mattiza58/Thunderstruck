# Thunderstruck
DIY Digital Multi-FX Guitar Pedal built with an ESP32S3 microcontroller

### Image of Prototype 
<p align = "center">
  <img width="50%" height="50%" alt="thunderstruck_img1" src="https://github.com/user-attachments/assets/962fa53d-a96f-4c10-a87c-0c972796a183" />
</p>

This is a project I built for the Milstein Program in Technology and Humanity Summer Program at Cornell Tech in NYC. I am a guitarist and love playing with guitar effects. I always wanted to create my own guitar pedal, especially a MultiFX one. Using the resources at Cornell Tech

## Video Demos

[![Video Title](https://img.youtube.com/vi/W_J3h4WjY30/0.jpg)](https://www.youtube.com/watch?v=W_J3h4WjY30)




List of Parts:
| Part     | Cost     | Link     |
| ---------  | ------  | ----------------  |
| ESP32S3 (Microcontroller)         | $10.39         |[DigiKey](https://www.digikey.com/en/products/detail/espressif-systems/ESP32-S3-DEVKITC-1-N32R16V/26658348?gclsrc=aw.ds&gad_source=1&gad_campaignid=20243136172&gbraid=0AAAAADrbLljvXLrLHlJvxffMWuCEtGga7&gclid=EAIaIQobChMIxJH83tW8lgMVZEhHAR2T_DAYEAQYBiABEgIO4_D_BwE)|
| PCM5102A (DAC)          | $8.88        | [Amazon](https://www.amazon.com/PCM5102-PCM5102A-Digital-Converter-Raspberry/dp/B0DNW32Y46/ref=sr_1_3?dib=eyJ2IjoiMSJ9.3opmNSuydIj_YHWcnbXBNjQN5WC2wKZC7IuAjR7cHF1XLUp6jSqC3NI68MkmkD6mSBtJVsUxV7Ulsrq5V15MfomZ1XzbNHkBAHCx-zFtGxEDQG7xbVCUyjxCG5nd68kgKGKSjI69oHPHkrF9G44JPb49IY2w-qFocynm4Ph8WSOwrPIrluHIO_DzgaT--Eem-VAb_53jW-hNkuEBnt3pOlkcnfmsv4KRsZfoipqQS5E.PYoSd8p3-3iVrLpX0RiICShh_0qbkHiBR5r0XfkagUQ&dib_tag=se&keywords=pcm5102a&qid=1782919726&sr=8-3)                 |
| PCM1808 (ADC)       | $6.99       | [Amazon](https://www.amazon.com/PCM1808-Single-Ended-Analog-Input-Decoder-Amplifier/dp/B0D9LNGBD1/ref=sims_dp_d_dex_ai_rank_model_1_d_v1_d_sccl_1_1/134-0589504-8391536?pd_rd_w=Fw1Ek&content-id=amzn1.sym.bb4a0aac-c2b4-4b4b-a0c8-9aa89b28dce3&pf_rd_p=bb4a0aac-c2b4-4b4b-a0c8-9aa89b28dce3&pf_rd_r=90KYF1Z0MDPM0PP7H72W&pd_rd_wg=Ij5CE&pd_rd_r=037f83c3-6de7-4ee7-b614-ec25cb448b78&pd_rd_i=B0D9LNGBD1&psc=1)                |
| 1/4 inch Mono Guitar Jacks       | $5.97         | [Amazon](https://www.amazon.com/Heams-Upgraded-Multi-Contact-Contact-Appearance/dp/B0F48FQ97D/ref=sr_1_1_sspa?crid=1B43XV2BHWG25&dib=eyJ2IjoiMSJ9.YG1P_H4dEnj9qErmaqWUNJQ3xsrWpviFSkc_75AxrJXU9nF9Gn9WP4-FunZ-cON6Ie1IiJIS6sWdki0nCPkOPB-k3zuU6DswL2gADwE6dJFvePBhyI5jWpStThmpszBWE9HUiLqrjjfQ13TD74Zm4JaZPGtOLCVAAILGKPeGDbeiMeOSvFH0L5dcT2o_BlVzb1MTdWV8Igbuw4X8PwE-9wjyd0pukKrVALM7bdZfN1s.jl4oGjYToNnzIoRdLcyX9sT_24Zh7PtaNsqxOA3vWME&dib_tag=se&keywords=1%2F4%22+Mono+Jacks&qid=1782870287&sprefix=1%2F4+mono+jacks+%2Caps%2C110&sr=8-1-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&psc=1)              |
| Rotary Encoders      | $8.88       | [Amazon](https://www.amazon.com/WWZMDiB-Encoder-Digital-Potentiometer-Arduino/dp/B0C6Q67V97/ref=sr_1_3?crid=2WSMFZFLEOAP&dib=eyJ2IjoiMSJ9.0MJCe5ZpwCSg0dFcPZlwUyuS5wb-jFX-e4-OT88-CwRh704lH9aark8XbcrNfEenzqXafOuLUVXsq1ZV5HElVR7ePpSVbuSjhBle8Q8TlUguxQkghR7KFOivg2zPL18zkm0wZ92qMBYXu0hwa6qMfHj-gjZ8_eOHmeooDTMGIaJp7O48AQH2KCBEwUtxSNkmEd4wkCB52jUS7R-trSw1f_U34K-S0chGQApLUMhshwI.W5y2mF8bXRRqdne-iRf84-ih7U6tT0iGnfKgEsJxotM&dib_tag=se&keywords=rotary+encoder&qid=1782916669&sprefix=rotary+enco%2Caps%2C128&sr=8-3)                 |
| 3.5in LCD Touchscreen Display    | $18.99         | [Amazon](https://www.amazon.com/Hosyond-320x480-Capacitive-Display-Mega2560/dp/B0CMD7Y55M/ref=sr_1_3?crid=2LW765V3ABW50&dib=eyJ2IjoiMSJ9._5hqrDNUH2Q6KCi4Ju58fLDTTDEXK113bVgZBeiK7e7R-X6mAR5fYYfQUt6N5R_j_-gmAWkN8yZBr-tP2EBlLtHCh0VVfQLMGLjAGao9g7jeNOeGIMPs9W1QgakGxfSZgb_ltjH0FS_mQOSoakh1Y1gVbNun34jHhVwE2PEe7l1RsuiRFylG13vJUOQJWrs9qjeMjFYtB44omV6qihYeldahjZPWG_lrbWQYizoXA2M.k2i_lOqkH7aP8dHZOH8uBgRdzqzuNpX0yGrnpKx5i6U&dib_tag=se&keywords=capacitive%2Blcd%2Btouchscreen&qid=1782917746&sprefix=capicative%2Blcd%2Btouchscreen%2Caps%2C125&sr=8-3&th=1)               |
| 3PDT Footswitch    | $7.88       | [Amazon](https://www.amazon.com/Footswitch-Guitar-Effects-Switch-Bypass/dp/B0DQWB53ZM/ref=sr_1_5?dib=eyJ2IjoiMSJ9.fR8U6hWSuYSaJZa2-GjKsZcMZ5zs3PoJQNJk3QlbRolflI6XGzaBqCY8JDopFZz5tVoL-jcRwzJ5IPtKtjOmfpSCTc3EnkOO7p8tIlrhBWimJ4gP-ysDeVg1q0hNCMN3p4UWwHcTWPeOFl8dF1YdhS4SPfAG_VW9PHuu8j4keBwBad7jfHarOxGGqJBpwnyzuMbFzQQNosVf3_ih-omoAEdfiz-RwHCpUFsfHPwIKt3RKtGAdXAgQxmENHYMbQ4hDoykhu2G2PRkA3GvrY5cLZRFW8NWUpoedM5ma1qqwwg.FkIGvqm5mZdSAx-SB11DdydWdXZTy7UJTrWGY8xbkcI&dib_tag=se&keywords=guitar+pedal+switch&qid=1782917399&sr=8-5)                 |

Pin Outs:
(NOTE: These are the pinouts I used)
| Connection     | Pin    | ESP32S3 GPIO   |
| ---------  | ------  | ----------------  |
| ADC | BCK | 12 |
| ADC | LRC | 14 |
| ADC | SCK | 42 |
| ADC | OUT | 41 |
| ADC | VCC | 5V |
| ADC | GND | GND |
| ADC | GND | GND |
| DAC | LIN  | Jack Tip |
| DAC | RIN  | None (Mono) |
| DAC | GND | 3.3V |
| DAC | SCK  | GND |
| DAC | BCK | 12 |
| DAC | DIN | 13 |
| DAC | LCK | 14 |
| LCD | VCC | 5V |
| LCD | GND | GND |
| LCD | LCD_CS | 18 |
| LCD | LCD_RST | 9 |
| LCD | LCD_RS | 8 |
| LCD | SDI (MOSI) | 16 |
| LCD | SCK | 15 |
| LCD | LED | 10 |
| LCD | SDO (MISO) | 17 |
| LCD | CTP_SCL | 2 |
| LCD | CTP_RST | 21 |
| LCD | CTP_SDA | 1 |
| LCD | CTP_INT | 47 |







