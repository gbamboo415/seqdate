// seqdate.c
// 指定された期間の日付を改行区切りで出力する
// 入力: 2つの引数

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INDIVIDUAL_ARG_MAX_LENGTH       64		  // 日付表現文字列の最大長
#define YEAR_MONTH_DAY_STRING_MAX_LEN   16
#define SECONDS_PER_DAY                 60*60*24

struct tm parse_datestring(char *datestr);

int main(int argc, char *argv[]) {
	char inputtext_startdate[INDIVIDUAL_ARG_MAX_LENGTH + 1], inputtext_enddate[INDIVIDUAL_ARG_MAX_LENGTH + 1];
	struct tm tm_startdate,                                  tm_enddate;
	time_t tt_startdate,                                     tt_enddate;

	time_t     t;
	struct tm *tm_output_date;
	int        out_year, out_month, out_day;

	if (argc != 3) {
		fputs("Message: 開始日と終了日、2つの引数を指定してください。\n", stderr);
		exit(EXIT_FAILURE);
	}

	// 引数文字列をコピーする
	strncpy(inputtext_startdate, argv[1], INDIVIDUAL_ARG_MAX_LENGTH);
	strncpy(inputtext_enddate,   argv[2], INDIVIDUAL_ARG_MAX_LENGTH);
	inputtext_startdate[INDIVIDUAL_ARG_MAX_LENGTH] = '\0';
	inputtext_enddate  [INDIVIDUAL_ARG_MAX_LENGTH] = '\0';

	// 日付文字列の解析
	tm_startdate = parse_datestring(inputtext_startdate);
	tm_enddate   = parse_datestring(inputtext_enddate);

	// time_t型に変換
	tt_startdate = mktime(&tm_startdate);
	tt_enddate   = mktime(&tm_enddate);

	if ((tt_startdate == (time_t)-1) || (tt_enddate == (time_t)-1)) {
		fputs("Error: 時刻変換中にエラーが発生しました。\n", stderr);
		exit(EXIT_FAILURE);
	}

	// 期間中の日付を出力
	for (t = tt_startdate; t <= tt_enddate; t += SECONDS_PER_DAY) {
		tm_output_date = localtime(&t);

		out_year  = tm_output_date->tm_year + 1900;
		out_month = tm_output_date->tm_mon  + 1;
		out_day   = tm_output_date->tm_mday;

		printf("%4d-%02d-%02d\n", out_year, out_month, out_day);
	}

	return EXIT_SUCCESS;
}

struct tm parse_datestring(char *datestr) {
	char       *str_year, *str_month, *str_day;
	int        year, month, day;
	struct tm  tm_ret;
	int        is_memory_allocated = 0;

	// 最初に日付文字列を区切り文字列で分割できないか試す
	if ((strchr(datestr, '/') == NULL) && (strchr(datestr, '-') == NULL)) {
		// 区切り文字が見つからない場合、yyyymmdd形式とみなす
		// メモリ領域を確保
		str_year  = (char *)malloc(sizeof(char) * YEAR_MONTH_DAY_STRING_MAX_LEN);
		str_month = (char *)malloc(sizeof(char) * YEAR_MONTH_DAY_STRING_MAX_LEN);
		str_day   = (char *)malloc(sizeof(char) * YEAR_MONTH_DAY_STRING_MAX_LEN);
		if ((str_year == NULL) || (str_month == NULL) || (str_day == NULL)) {
			fputs("Error: メモリ確保に失敗しました。\n", stderr);

			free(str_year);
			free(str_month);
			free(str_day);

			exit(EXIT_FAILURE);
		}
		is_memory_allocated = 1;

		// 念のため文字数をチェック
		if ((int)strlen(datestr) == 8) {
			strncpy(str_year,  &datestr[0], 4);
			strncpy(str_month, &datestr[4], 2);
			strncpy(str_day,   &datestr[6], 2);
		}
	} else {
		// 区切り文字が存在するので、区切り文字で日付文字列を分割する
		str_year  = strtok(datestr, "/-");
		str_month = strtok(NULL, "/-");
		str_day   = strtok(NULL, "/-");
	}

	year  = atoi(str_year);
	month = atoi(str_month);
	day   = atoi(str_day);

	tm_ret.tm_year = year  - 1900;
	tm_ret.tm_mon  = month - 1;
	tm_ret.tm_mday = day;
	tm_ret.tm_wday = 0;
	tm_ret.tm_hour = 0;
	tm_ret.tm_min  = 0;
	tm_ret.tm_sec  = 0;
	tm_ret.tm_yday = 0;
	tm_ret.tm_isdst   = 0;

	// yyyymmdd形式で分割した場合に限り、動的に確保したメモリを解放
	if (is_memory_allocated == 1) {
		free(str_year);
		free(str_month);
		free(str_day);
	}

	return tm_ret;
}
