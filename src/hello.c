#include"hello.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#ifdef _MSC_VER
#include<intrin.h>
#include<conio.h>
#else
#include<x86intrin.h>
#endif
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>

#define SRC_FN "rises.txt"
#define SRC_PATH "C:/Projects/hello/"

void move_cursor(int dx, int dy)
{
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &coninfo);
	coninfo.dwCursorPosition.X+=dx;
	coninfo.dwCursorPosition.Y+=dy;
	SetConsoleCursorPosition(hConsole, coninfo.dwCursorPosition);
}
int print_string_1sp(const char *str, int start, int end, int req_chars)//returns new idx
{
	int idx=start, nchars=0;
	for(;;++nchars)
	{
		if(end==-1)
		{
			if(nchars>=req_chars)
				break;
		}
		else
		{
			if(idx>=end)
				break;
		}
		int c=isspace(str[idx])?' ':str[idx];
		printf("%c", c);
		++idx;
		if(isspace(c))
			for(;nchars<req_chars&&isspace(str[idx]);++idx);
	}
	return idx;
}
int main(int argc, const char **argv)
{
	ArrayHandle text=0;
	if(argc==2)
		text=load_file(argv[1], 0, 0, 0);
	if(!text)
		text=load_file(SRC_FN, 0, 0, 0);
	if(!text)
		text=load_file(SRC_PATH SRC_FN, 0, 0, 0);
	if(!text)
	{
		printf("Pass some large text file as a command argument.\n");
		goto game_over;
	}

	printf(
		"Hello Zipf,\n\n"
		"    I want to play a game.\n"
		"    The rules are simple. "
		"I reveal the words one bit at a time, "
		"and you enter the probability (from 0 to 10) that the next bit is TRUE. "
		"If expecting FALSE, enter a number from 1 to 4. "
		"If expecting TRUE, enter a number from 6 to 9. "
		"If not sure enter a 5. "
		"When 100%% certain, you can take a risk by entering 0 or \'A\'. "
		"When cetrain about all remaining bits, you can press \'R\' followed by the expected symbol. "
		"To bypass-code whole symbol press \'B\'.  "
		"You will need the ASCII table below.\n\n"
		"    Good Luck.\n\n"
	);
	for(int ky=0;ky<19;++ky)//print ASCII table
	{
		for(int kx=0;kx<5;++kx)
		{
			int c=32+19*kx+ky;
			printf("    ");
			for(int kb=6;kb>=0;--kb)
				printf("%d", c>>kb&1);
			printf(" %c", c);
		}
		printf("\n");
	}
	printf(
		"\n"
	//	"Remember the number row:\n"
		"    FALSE <-- 0 1 2 3 4 5 6 7 8 9 A --> TRUE\n\n"
	);

	{
		const int prob_den=10, hint_size=100, puzzle_size=10;
		double usize=0, csize=0;
		srand((unsigned)__rdtsc());
		int start, idx, end, nrevealed;
		do
		{
			start=(rand()<<15^rand())%text->count;
		}while(start>text->count-(hint_size+puzzle_size)*2);

		nrevealed=hint_size;
		idx=start;
		end=start+hint_size+puzzle_size;

		//hint=idx+hint_size;
		//end=hint+puzzle_size;

		//printf("Hint: \"");
		//idx=print_string_1sp(text->data, idx, -1, hint_size);
		//for(;idx<hint;)//print hint
		//{
		//	printf("%c", isspace(text->data[idx])?' ':text->data[idx]);
		//	++idx;
		//	if(isspace(text->data[idx-1]))
		//		for(;idx<text->count&&isspace(text->data[idx]);++idx);
		//}
		//printf("\"\n");

		for(;;++nrevealed)//GAME LOOP
		{
			printf("...");
			idx=print_string_1sp(text->data, start, -1, nrevealed);
			printf("...\n");
			if(idx>=text->count||nrevealed>=hint_size+puzzle_size)
				break;
			int c=isspace(text->data[idx])?' ':text->data[idx];
			int lo=0, hi=128;
			int risk_all=0, bypass=0;
			for(int kb=6;kb>=0;--kb)
			{
				printf("Bit %d: ", kb);
				for(int kb2=6;kb2>kb;--kb2)
					printf("%d", c>>kb2&1);
				printf("[?]");
				for(int kb2=kb-1;kb2>=0;--kb2)
					printf("?");
				printf("    ");

				printf("\'%c\' ~ \'%c\'    ", CLAMP(32, lo, 126), CLAMP(32, hi-1, 126));
				double p;
				int bit=c>>kb&1;
				if(bypass)
					p=0.5;
				else if(risk_all)
					p=(risk_all>>kb&1)==bit;
				else
				{
//#ifdef _MSC_VER
					int p1=0;
					do
					{
						printf("P(1): ");
						p1=_getche();
						if((p1&0xDF)=='B')
						{
							bypass=1;
							break;
						}
						if((p1&0xDF)=='R')
						{
							printf("  Symbol: ");
							risk_all=_getche();
							break;
						}
						p1=p1>='0'&&p1<='9'?p1-'0':(p1&0xDF)-'A'+10;
					}while(p1<0||p1>prob_den);
//#else
//					int p1=0;
//					do
//					{
//						printf("P(1): ");
//						if(!scanf("%d", &p1))
//							continue;
//					}while(p1<=0||p1>=prob_den);
//#endif
					if(bypass)
						p=0.5;
					else if(risk_all)
						p=(risk_all>>kb&1)==bit;
					else
						p=bit?p1/(double)prob_den:1-p1/(double)prob_den;
				}
				++usize;
				if(fabs(p)<1e-4)
				{
					printf("\n\n");
					switch(rand()&7)
					{
					case 0:printf("Your computer is now a black hole\n  GAME OVER\n");break;
					case 1:printf("  GAME OVER  //TODO: Turn computer into a black hole\n");break;
					case 2:printf("After this operation all that remains is a huge crater\n  GAME OVER\n");break;
					case 3:printf("KABOOM!\n  GAME OVER\n");break;
					case 4:printf("You were 100%% certain but turned out wrong. The laws of physics don\'t allow this kind of wavefunction collapse. It\'s like finding out that Schrodinger\'s cat disappeared without a trace, and now there is Cthulhu in the box. No one knows wh...\n  Universe.exe has stopped working\n");break;
					case 5:printf("GAME OVER\n");break;
					case 6:printf("GAME OVER\n");break;
					case 7:printf("GAME OVER\n");break;
					}
					printf("\n\n");
					goto game_over;
				}
				double bitsize=-log2(p);
				csize+=bitsize;

				if(bit)
					lo+=1<<kb;
				else
					hi-=1<<kb;

				if(p<0.5)
					printf("\nConcentrate!\n");

				//move_cursor(0, -1);
				//for(int k=0;k<64;++k)
				//	printf("_");
				printf("\r");
			}
			for(int kb2=6;kb2>=0;--kb2)
				printf("%d", c>>kb2&1);
			printf("    \'%c\'    Score: %lf / %lf = %lf\n", c, usize/7, csize/7, usize/csize);
			
			//idx=print_string_1sp(text->data, start, -1, hint_size);
			//++idx;
			//if(isspace(c))
			//	for(;idx<text->count&&isspace(text->data[idx]);++idx);
		}
		printf(
			"Score  %lf / %lf = %lf\n"
			"  %s\n",
			usize/7, csize/7, usize/csize, csize?csize<usize?"Congratulations! You Won!":"Bad Job! Game Over!":"Congratulations! You compressed that down to 0 bytes! May be you should consider deleting files as a form of data compression?"
		);
	}
game_over:
	//printf("Game Over.\n");
	pause();
	return 0;
}
