#include <stdio.h>
#include <stdbool.h>
 
#define C_LIM_135Y (unsigned char)135  // Лимит 135 лет.
 
extern const unsigned short C_DAYTAB[ 13 ];
 
typedef struct DataTime_tag
{
    unsigned char uc_sec;
    unsigned char uc_min;
    unsigned char uc_hour;
 
    unsigned char uc_day;
    unsigned char uc_month;
    unsigned char uc_year;
}DataTime;
 
 
 
unsigned long ul_set_sec = 3150000;
 
 
void SecToDateTime( unsigned long ul_set_sec, DataTime* datatime);
 
unsigned char LeapYears( unsigned char uc_year );
 
bool IsLimitSec( unsigned long ul_set_sec );
 
bool ExcessYear( unsigned char uc_year, unsigned short us_days_sec );
 
bool ExcessMonth( unsigned short us_approx_month, unsigned short us_days_out_year);
 
bool IsLeapYear (unsigned char uc_year);
 
 
 
 
int main( void )
{
    struct DataTime_tag dt;
 
    SecToDateTime(ul_set_sec, &dt);
    printf("SECOND %ld\n\n", ul_set_sec);
    printf("DATE FROM SECOND: ");
    printf("20%.2d.", dt.uc_year);
    printf("%.2d.", dt.uc_month);
    printf("%.2d\n\n", dt.uc_day);
    printf("TIME FROM SECOND: ");
    printf("%.2d:%.2d:%.2d", dt.uc_hour, dt.uc_min, dt.uc_sec);
    return 0;
}
 
const unsigned short C_DAYTAB[ 13 ] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
 
 
/*===================================================================================
 *
 *      SecToDateTime
 *
 *===================================================================================
 *  Секунды преобразует в дату и время( через указатели ).
 *
 *  Описание:   секунды преобразует в дни, из дней рассчитывает год, из остатка дней
 *      которые не входят в год, рассчитывает месяц и день месяца. Далее рассчитывает
 *      из сек которые не вмещаются в день - время.
 *
 * Вход/Выход:  (глобальная переменная) ul_set_sec, (Структура) DataTime c указателем
 *      на структуру datatime.
 */
void SecToDateTime( unsigned long ul_set_sec, DataTime* datatime)
{
    unsigned short us_days_out_year;
    unsigned short us_days_sec;
    unsigned short us_days_year;
    unsigned long  ul_sec_out_days;
 
    // ---> Проверка на лимит секунд.
    if( IsLimitSec( ul_set_sec ) )   return;
 
    // ---> Рассчет суммарных дней.
    us_days_sec = ul_set_sec / ( 60*60*24 );
 
    // ---> Рассчет года.
    datatime->uc_year = us_days_sec / 365;                                                         // Год либо точный либо на 1 больше.
    datatime->uc_year -= ExcessYear( datatime->uc_year, us_days_sec );                             // Получаем точный год.
 
    // ---> Рассчет дней которые вмещаются в целые года.
    us_days_year = 0;
    if( datatime->uc_year > 0 ) us_days_year = ( datatime->uc_year * 365 ) + LeapYears( datatime->uc_year );          // Кол-во дней которые помещаются в года.
 
    // ---> Расчет дней которые НЕ вмещаются в целый год.
    us_days_out_year = 0;
    if( datatime->uc_year > 0 ) us_days_out_year = us_days_sec - us_days_year;
    else                        us_days_out_year = us_days_sec;
 
    // ---> Рассчитать месяц.
    datatime->uc_month = (unsigned char)( us_days_out_year / 29 ) + 1;                                                 // Примерный месяц, точен либо на 1 больше.
    // -> Корректировка месяца
    datatime->uc_month = (unsigned char)( datatime->uc_month - ExcessMonth( datatime->uc_month, us_days_out_year ) );  // Неточность с 12 и 0 месяцем дальше исправляется.
    if( (us_days_out_year > 334) && (us_days_out_year < 348) ) datatime->uc_month--;                 // Корректировка при неточности 12 месяца.
    if( datatime->uc_month == 0 ) datatime->uc_month++;                                              // Корректировка если месяц 0.
 
    // ---> Расчитать дни месяца.
    datatime->uc_day = (unsigned char)( us_days_out_year - C_DAYTAB[ datatime->uc_month - 1 ] ) + 1;  // Примерный день, точен либо на 1 меньше.
    if( (IsLeapYear( datatime->uc_year )) && ( datatime->uc_month < 3 ) ) datatime->uc_day++;         // Коррекция. получаем упущеный день.
 
    // ---> Расчитать время.
    ul_sec_out_days = 0;
    ul_sec_out_days = ( ul_set_sec - ( us_days_sec * (60*60*24)) );
    // Час, далее минуты, далее секунды.
    datatime->uc_hour = (unsigned char)(  ul_sec_out_days / ( 60*60 ));
    datatime->uc_min  = (unsigned char)(( ul_sec_out_days /  60 ) % 60);
    datatime->uc_sec  = (unsigned char)(  ul_sec_out_days %  60 );
 
}
 
 
/*========================================================================================
 *
 *          IsLimitSec
 *
 *========================================================================================
 * Возвращает 1(TRUE) если сек выше лимита и 0(FALSE) Если не привышает лимит(135лет).
 *   (Лимит задается в годах)
 *
 * Вход (Глобальная переменная) ul_set_sec.
 *
 */
bool IsLimitSec( unsigned long ul_set_sec )
{
    unsigned char uc_lim_years;
    unsigned long ul_total_leap_years;
    unsigned long ul_total_usual_years;
    unsigned long ul_total_sec;
 
    // ---> Получаем кол-во годов.
    uc_lim_years = C_LIM_135Y;
    ul_total_leap_years = LeapYears( uc_lim_years );
    ul_total_usual_years = uc_lim_years - ul_total_leap_years;
 
    // ---> Из годов получаем секунды.
    ul_total_leap_years *= (366*24*60*60);                         // Сек из високосных годов.
    ul_total_usual_years *= (365*24*60*60);                        // Сек из обычных годов.
    ul_total_sec = ul_total_leap_years + ul_total_usual_years;
 
    //--------- Сравниваем результаты секунд(заданых и лимита). ----------->
    if( ul_set_sec > ul_total_sec )  return 1;
    return 0;
}
 
/*===================================================================================
 *
 *      LeapYears
 *
 *===================================================================================
 *  Возвращает кол-во високосных годов. (Работоспособность до 400 годов).
 *
 *  Вход: (Глобальная переменная) uc_year.
 */
unsigned char LeapYears( unsigned char uc_year )
{
    return ( ( ( uc_year / 4 ) + 1 ) - ( uc_year / 100 ) );  // учитывает 2000 как високосный.
}
 
/*===================================================================================
 *
 *      ExcessYear
 *
 *===================================================================================
 *
 *  Возвращает лишнее кол-во годов.
 *
 *  Вход: (Глобальная)uc_year, (Локальная)us_days_sec.
 */
bool ExcessYear( unsigned char uc_year, unsigned short us_days_sec )
{
    unsigned short us_days_year = 0;
 
    us_days_year = (uc_year * 365) + LeapYears( uc_year );
    if( us_days_year > us_days_sec ) return 1;
    return 0;
}
 
 
/*===================================================================================
 *
 *      ExcessMonth
 *
 *===================================================================================
 *
 *  Возвращает лишнее кол-во месяцев.
 *
 *  C_DAYTAB[ 13 ] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
 *
 *  Вход:  us_approx_month, us_days_out_year.
 *
 */
bool ExcessMonth( unsigned short us_approx_month, unsigned short us_days_out_year)
{
 
    if( us_approx_month > 12 ) return 1;
    if( us_approx_month == 1 ) return 0;        // 1 месяц точный.
    if(
        ( us_days_out_year > C_DAYTAB[ us_approx_month - 1 ] )   &&
        ( us_days_out_year < C_DAYTAB[ us_approx_month + 1 ] )
      ) return 0;
    return 1;
 
}
 
/*===================================================================================
 *
 *       IsLeapYear
 *
 *===================================================================================
 *
 * IsLeapYear
 *
 * Возвращает 1(TRUE) если год високосный, иначе 0(FALSE). (2000 учитывает високосным)
 *
 * Вход: (глобальная переменная): uc_year.
 */
bool IsLeapYear (unsigned char uc_year)
{
    if( uc_year == 0 ) return 1;
    if( ( uc_year % 4 == 0) && (uc_year % 100 != 0) ) return 1;
   return 0;
}
