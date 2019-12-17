/*
 * File:   CounterDate.c
 * Author: xodsergey
 *
 * Created on 15 октября 2018 г., 15:39
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
 
#define C_29DAY_IN_FEB 29
#define C_28DAY_IN_FEB 28
#define C_BEG_YEARS2000 2000
#define C_END_YEARS2170 2170
#define C_365DAYS_IN_YEAR 365
#define C_3600SEC_IN_HOUR 3600
#define C_60SEC_IN_MINUTE 60
#define C_86400SEC_IN_DAY 86400
 
 
//-------------> Задать дату.
unsigned short us_year   = 2001;
unsigned char  uc_month  = 0,
               uc_day    = 0,
               uc_hour   = 0,
               uc_minute = 0,
               uc_second = 0;
 
 
 
 unsigned long long int ConvertDateInSeconds(unsigned short us_year, unsigned char uc_month, unsigned char uc_day, unsigned char uc_hour, unsigned char uc_minute, unsigned char uc_second);
 unsigned char DetermineNdaysMonth(unsigned char uc_month);
 bool GetCalculationLeapYear (unsigned short us_year);
 bool ValidateDate(unsigned short us_year, unsigned char uc_month, unsigned char uc_day, unsigned char uc_hour, unsigned char uc_minute, unsigned char uc_second);
 unsigned short GetAllLeapYear (unsigned short us_year);
 
 
int main()
{
   ConvertDateInSeconds(us_year, uc_month, uc_day, uc_hour, uc_minute, uc_second);
  return 0;
}
 
/*==============================================================================
 *
 *                        Преоброзовать дату в секунды
 *
 *==============================================================================
 * ConvertDateInSeconds
 *
 * Преобразует дату в секунды. на выходе возвращает секунды. если дата неверна
 *  возвращает 0;
 * (Считает дату с 2000 года до 2170, 2000 год в счет не идет).
 *
 * Использует след функции:
 *      DetermineNdaysMonth, GetCalculationLeapYear, ValidateDate.
 *
 * Использует след переменные:
 *      us_year, uc_month, uc_day, uc_hour, uc_minute, uc_second.
 */
unsigned long long int ConvertDateInSeconds(unsigned short us_year, unsigned char uc_month, unsigned char uc_day, unsigned char uc_hour, unsigned char uc_minute, unsigned char uc_second)
{
    //=========================================================================>
    //
    unsigned short     us_cnt_n_days_months = 0;       //Счетчик суммарных дней за N месяцев.
    unsigned long long int  ulli_total_days = 0;       //Для хранения всех дней.
    //----------> Дата для преоброзования в сек.
    unsigned int   ui_day    = 0,
                   ui_hour   = 0;
    unsigned short us_minute = 0;
 
    //=========================================================================>
    if( ValidateDate ( us_year, uc_month, uc_day, uc_hour, uc_minute, uc_second ) )
    {
        //=====================================================================>
        //-----> Меняется значение года в зависимости от месяца.
        if      ( uc_month  > 0 )   us_year = (us_year - 1) - C_BEG_YEARS2000;
        else if ( uc_month == 0 )   us_year -= C_BEG_YEARS2000;
        //=====================================================================>
        //-----> Преоброзовать года в дни и прибавить все високосные года.
        ulli_total_days = us_year * C_365DAYS_IN_YEAR + GetAllLeapYear ( us_year );
 
        //=====================================================================>
        //-----> Вычислить общее кол-во дней до N месяца.
        if( uc_day == 0 )
        {
            for(unsigned char i = 1; i <= uc_month; i++)
            {
             us_cnt_n_days_months += DetermineNdaysMonth(i);
            }
        }
        else
            for(unsigned char i = 1; i < uc_month; i++)
            {
             us_cnt_n_days_months += DetermineNdaysMonth(i);
            }
 
        //=====================================================================>
        //-----> Прибавить дни месяцев ко всем вычислинным дням.
        ulli_total_days += us_cnt_n_days_months;
 
        //=====================================================================>
        //-----> Преоброзование дней, часов, минут в секунды.
        ulli_total_days       *= C_86400SEC_IN_DAY;
        ui_day    += uc_day    * C_86400SEC_IN_DAY;
        ui_hour   += uc_hour   * C_3600SEC_IN_HOUR;
        us_minute += uc_minute * C_60SEC_IN_MINUTE;
 
        return ulli_total_days + ui_day + ui_hour + us_minute + uc_second;
    }
    return 0;
 
}
 
/*==============================================================================
 *
 *                    Check on adequacy / Проверка на адекватность
 *
 *==============================================================================
 *
 * Проверяет дату на адекватность, если все впорядке возвращает 1(TRUE) иначе 0(FALSE)
 * (При проверке Год ограничен от 2000 до 2170)
 * Входные переменные: us_year, uc_month, uc_day, uc_hour, uc_minute, uc_second.
 */
bool ValidateDate(unsigned short us_year, unsigned char uc_month, unsigned char uc_day, unsigned char uc_hour, unsigned char uc_minute, unsigned char uc_second)
{
    //=========================================================================>
    //-----> Проверка, не привышает ли дата Макс значения.
    if(
        ( us_year  == C_BEG_YEARS2000 )                               ||
        ( us_year   > C_END_YEARS2170 )                               ||
        ( uc_month  > 12 )                                            ||
        ( uc_day    > DetermineNdaysMonth ( uc_month ) )              ||
        ( uc_hour   > 24 )                                            ||
        ( uc_minute > 60 )                                            ||
        ( uc_second > 60 )
      ) return 0;
    //=========================================================================>
    //-----> Не пропускает заполнение часов, минут, и сек, если день равен 0.
    if( uc_day == 0 )
    {
        if(
            ( uc_hour   > 0 )  ||
            ( uc_minute > 0 )  ||
            ( uc_second > 0 )
          ) return 0;
    }
    //=========================================================================>
    //-----> Не пропускает заполнение минут и сек если час равен 0.
    if( uc_hour == 0 )
    {
        if(
            ( uc_minute > 0 )  ||
            ( uc_second > 0 )
          ) return 0;
    }
    //=========================================================================>
    //-----> Не пропускает заполнение сек, если минуты равны 0.
    if( uc_minute == 0 && uc_second > 0) return 0;
 
    return 1;
 
}
 
 
/*==============================================================================
 *
 *                 Determine Month/ Определить месяц
 *
 *==============================================================================
 *
 * Возвращает кол-во дней в указаном месяце.
 *
 * Получает месяц проверяет его на кол-во дней.
 *
 * Входные переменные us_month
 *  Используемые функции CalculationLeapYear.
 *
 */
unsigned char DetermineNdaysMonth(unsigned char uc_month)
{
    //=========================================================================>
    switch ( uc_month )
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 2:
            if( GetCalculationLeapYear(us_year) ) return 29;
            return 28;
    default:
        return 0;
 
    }
}
 
 
/*==============================================================================
 *
 *                    CLY / Вычислить високосный ли год
 *
 * =============================================================================
 * GetCalculationLeapYear
 *
 * Вычисляет является ли указанный год високосным или нет.
 * Если високосный возвращает 1(TRUE) если не высокосный 0(FALSE).
 * (считает со всеми исключениями григорианского календаря).
 *
 * Входные переменные us_year.
 *
 */
bool GetCalculationLeapYear (unsigned short us_year)
{
    //=======================================================>
    //----> Если это условие верно значит год обычный.
    if(
        ( us_year %   4 != 0 )                        ||
        ( us_year % 100 == 0 && us_year % 400 != 0)
      ) return 0;
    return 1;
}
 
/*==============================================================================
 *
 *             GetAllLeapYear / Получить все високосные года
 *
 *==============================================================================
 * GetAllLeapYear
 *
 * Возвращает суммарное кол-во високосных годов.
 *
 * Входные переменные: us_year.
 *
 */
unsigned short GetAllLeapYear (unsigned short us_year)
{
   return us_year = (us_year / 4) - (us_year /100);
}
