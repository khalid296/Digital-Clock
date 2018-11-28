# Digital-Clock

[![Watch the video](https://img.youtube.com/vi/CfxZx5thjB0/maxresdefault.jpg)](https://youtu.be/CfxZx5thjB0)

Digital clock with date(day, month, year, day of week),
time(hour, minute, second ),
temperature and alarm(hour, minute, day of week) is controlled using push buttons for select, back ,+ and - to set date, time and alarm. 
alarm is saved on internal EEPROM, date, time are from external I2C DS1307 and temperature is from TC72 SPI sensor.
when alarm match time(hour, minute, day of week) buzzer work for 1 minute.
