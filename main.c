
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "helpers.h"
#include <time.h>
#include "skb.h"
#include "config.h"

#define green  "#00ff00"
#define red    "#ed3d40"
#define orange "#f4e71a"
#define blue   "#66a1f9"
#define white  "#ffffff"
#define gray   "#999999"
#define yellow "#ffff00"
#define reeed  "#E87605"
#define reeeed "#FF2301"

char *month[] =
{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};
char *week[] =
{
    "Sun","Mon","Tue","Wed","Thu","Fri","Sat"
};

void p_col(char* col)
{
    printf("\"color\":\"%s\"", col);
}
void p_com(char *com)
{
    printf("\"full_text\":\"%s\"", com);
}
void p_ful(char *t, char *c)
{
    printf("{");
    p_com(t);
    printf(",");
    p_col(c);
    printf("}");
}

char out[1035] = {0},
    inp[100] = {0};
void lang()
{
    char *out = skb_get();
    if(strncmp(out, "Eng", 3) == 0)
	p_ful("Eng", red);
    else
	p_ful("Rus", blue);
}

void timedate()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    printf("{\"full_text\":\"%s:%02d:%s:%d\",", week[tm.tm_wday], tm.tm_mday, month[tm.tm_mon], tm.tm_year + 1900);
    p_col(white);
    printf("},");

    printf("{\"full_text\":\"%02d:%02d:%02d\",", tm.tm_hour, tm.tm_min, tm.tm_sec);
    p_col(white);
    printf("}");
}
static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

void cpu_init(){
    FILE* file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
        &lastTotalSys, &lastTotalIdle);
    fclose(file);
}

double cpu_get(){
    double percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
        &totalSys, &totalIdle);
    fclose(file);

    if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
        totalSys < lastTotalSys || totalIdle < lastTotalIdle){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else{
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
            (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
    }

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    return percent;
}
char cpu_buf[10];
int cpu_i = 0;
void cpu_print()
{
    cpu_i ++;
    if(cpu_i % 3 == 1)
	sprintf(cpu_buf, "CPU %d%", (int)cpu_get());
    p_ful(cpu_buf, white);
}
int parseLine(char* out){
    // This assumes that a digit will be found and the out ends in " Kb".
    int i = strlen(out);
    const char* p = out;
    while (*p <'0' || *p > '9') p++;
    out[i-3] = '\0';
    i = atoi(p);
    return i;
}

void ram_get()
{
    FILE* file = fopen("/proc/meminfo", "r");
    int result = -1;
    int tot, fre, ava;

    while (fgets(out, 1024, file) != NULL){
        if (strncmp(out, "MemTotal:", 6) == 0){
            tot = parseLine(out);
        } else
	if (strncmp(out, "MemFree:", 6) == 0){
            fre = parseLine(out);
	} else
	if (strncmp(out, "MemAvailable:", 6) == 0){
            ava = parseLine(out);
	    break;
	}
    }
    fclose(file);
    result = ava * 100 / tot;
    result = 100 - result;
    sprintf(out, "RAM: %d%%", result);
    p_ful(out, white);
    return result;
}

void cpu_temp()
{
    int t = 0;
    FILE* file = fopen(CPU_TEMP_PATH, "r");
    fscanf(file, "%d", &t);
    t /= 1000;
    fclose(file);
    sprintf(out, "CPU %d°C", t);
    if(t > 75)
	p_ful(out, red);
    else if(t > 65)
	p_ful(out, yellow);
    else
	p_ful(out, green);
}

void fan_speed()
{
    int t = 0;
    FILE* file = fopen(FAN_PATH, "r");
    fscanf(file, "%d", &t);
    fclose(file);
    if(t == 0)
	p_ful("F", gray);
    else if(t < 3000)
	p_ful("F", green);
    else if(t < 3800)
	p_ful("F", yellow);
    else
	p_ful("F", red);	
}

void bat()
{
    FILE* fp;
    char capacity[255], status[255];

    fp = fopen(BAT_CAP_PATH, "r");
    while(fgets(capacity, 255, (FILE*) fp)) {
    }
    capacity[strlen(capacity) - 1] = '\0';

    int ca = atoi(capacity);
    
    fclose(fp);

    fp = fopen(BAT_STAT_PATH, "r");
    while(fgets(status, 255, (FILE*) fp)) {
    }

    status[strlen(status) - 1] = '\0';
    char *col = white;
    if(status[0] != 'D')
	col = green;
    else if(ca < 20)
	col = red;
    else if(ca < 30)
	col = orange;
    else if(ca < 50)
	col = yellow;

    fclose(fp);

    p_ful(capacity, col);
}
void bat1()
{
    if( access(BAT1_CAP_PATH, F_OK ) == -1 ) {
        // file doesn't exist
        p_ful("NA", red);
        return;
    } 
    // file exists
    FILE* fp;
    char capacity[255], status[255];

    fp = fopen(BAT1_CAP_PATH, "r");
    while(fgets(capacity, 255, (FILE*) fp)) {
    }
    capacity[strlen(capacity) - 1] = '\0';

    int ca = atoi(capacity);
    
    fclose(fp);

    fp = fopen(BAT1_STAT_PATH, "r");
    while(fgets(status, 255, (FILE*) fp)) {
    }

    status[strlen(status) - 1] = '\0';
    char *col = white;
    if(status[0] != 'D')
	col = green;
    else if(ca < 20)
	col = red;
    else if(ca < 30)
	col = orange;
    else if(ca < 50)
	col = yellow;

    fclose(fp);

    p_ful(capacity, col);
}
char *ping = "nmap -sP --host-timeout 100ms ";
char *ips[] = {
    "5.255.255.55", //yandex
    "192.168.1.172",
    "192.168.1.100",
    "lutsai.ru"
};

int pings[4] = {0};
void fping()
{
    for (int i = 0; i < 1; i++) {
	out[0] = 0;
	inp[0] = 0;
	pings[i] = 0;
	strcat(inp, ping);
	strcat(inp, ips[i]);
	e_proc(inp, out, 1024);
	uint32_t r = 0, g = out + strlen(out);
	char *s = out;
//	printf("%s\n", s);
	while ( s < g) {
	    s ++;
	    if(strncmp("st is u", s, 7) == 0)
	    {
		pings[i] = 1;
		break;
	    }
	}
    }
}

void pr()
{
    fping();
    
    printf("[");
    /* if(NETWORK_REQUIRED) */
    /* { */
    /*     p_ful("E", pings[0] ? green : red); */
    /*     printf(","); */
    /* } */
    if(TEMP_REQUIRED)
    {
	cpu_temp();
	printf(",");
    }
    if(FAN_REQUIRED)
    {
	fan_speed();
	printf(",");
    }
    if(CPU_REQUIRED)
    {
	cpu_print();
	printf(",");
    }
    if(RAM_REQUIRED)
    {
	ram_get();
	printf(",");
    }
    if(BAT_REUIRED)
    {
      bat();
      printf(",");
      bat1();      
      printf(",");
    }
    if(LANG_REQUIRED)
    {
	lang();
	printf(",");
    }
    if(TIME_REQUIRED)
    {
	timedate();
    }
    printf("],\n");
}


int main() {
    if(LANG_REQUIRED)
	skb_init();
    if(CPU_REQUIRED)
	cpu_init();
    
    printf("{ \"version\": 1 }\n");
    printf("[ [],");    
    
    while(1)
    {
	pr();
	fflush( stdout );
	
	usleep(500000);
    }
  
    return 0;
}
