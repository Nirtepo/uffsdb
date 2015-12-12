//BufferPool
#include "buffend.h"
#include <ctype.h>

// FUNCOES AUXILIARES
int tamTupla(tp_table *esquema, struct fs_objects objeto) {// Retorna o tamanho total da tupla da tabela.

    int qtdCampos = objeto.qtdCampos, i, tamanhoGeral = 0;

    if(qtdCampos){ // Lê o primeiro inteiro que representa a quantidade de campos da tabela.
        for(i = 0; i < qtdCampos; i++)
            tamanhoGeral += esquema[i].tam; // Soma os tamanhos de cada campo da tabela.
    }

    return tamanhoGeral;
}

int pot10(int n) {
    if(n == 0)
        return 1;
    return 10 * pot10(n-1);
}

int strtam(char n[]) {
    if(n[0]==0)
        return 0;
    return 1+strtam(n+1);
}

int convertI(char u[]) {
    if(strtam(u) == 0)
        return 0;
    return (u[0]-48)*pot10(strtam(u)-1) + convertI(u+1);
}

double get_decimal(char u[]) {
    double decimal=0;
    int i,tamanho = strtam(u);
    for(i=0;i<tamanho && u[i]!='.';i++); // posiciona o indice i encima do ponto
    if (u[i] != '.') return 0;
    decimal=convertI(u+i+1);///(pot10(tamanho-i-1));
    decimal=(double)convertI(u+i+1)/(double)(pot10(tamanho-i-1));
    return decimal;
}

double get_inteiro(char v[]) {
    double inteiro=0;
    int i,tamanho = strtam(v);
    char u[10];
    strcpy(u,v); //copia o vetor para uma vaiável auxiliar para preservar sua originalidade
    for(i=0;i<tamanho && u[i]!='.';i++); // posiciona o indice i encima do ponto
    u[i]=0; // separa a parte inteira da parte decimal, inserindo um null no lugar do ponto
    inteiro=convertI(u);
    return inteiro;
}

double convertD(char u[]) {
    return get_inteiro(u)+get_decimal(u);
    //Soma inteiro com decimal.ss
}

double doubAbs(double n){
	if(n<0)
		return n*-1;
	else
		return n;
}

int compDtoD(double a, double b){
	if(((int)(doubAbs(a-b)*100000000))==0)
		return 0;
	else if(((int)(doubAbs(a-b)*100000000))>0)	
		return 1;
	else
		return -1;
}

int generalCmp(char *a, char *b, char t, int comp){
	int x;
	comp %=10;
	if(a==NULL||b==NULL||comp>5||comp<0)
		return -1;	
	if(t=='I'){
		int*I1 = (int *)a, *I2 = (int *)b;
		if(comp==0){
			if(*I1==*I2)
				return 1;
			else
				return 0;
		} 	
		else if(comp==1){
			if(*I1!=*I2)
				return 1;
			else
				return 0;		
		}
		else if(comp==2){
			if(*I1>*I2)
				return 1;
			else
				return 0;
		}
		else if(comp==3){
			if(*I1<*I2)
				return 1;
			else
				return 0;
		}
		else if(comp==4){
			if(*I1>=*I2)
				return 1;
			else
				return 0;
		}
		else if(comp==5){
			if(*I1<=*I2)
				return 1;
			else
				return 0;
		}
		else
			return -1;
	}
	else if(t=='S'||t=='D'){
		double *D1 = (double *)a,*D2 = (double *)b;
		x = (t=='S'? strcmp(a,b) : compDtoD(*D1,*D2));
		if(comp==0){
			if(x==0)
				return 1;
			else
				return 0;
		} 	
		else if(comp==1){
			if(x!=0)
				return 1;
			else
				return 0;		
		}
		else if(comp==2){
			if(x>0)
				return 1;
			else
				return 0;
		}
		else if(comp==3){
			if(x<0)
				return 1;
			else
				return 0;
		}
		else if(comp==4){
			if(x>=0)
				return 1;
			else
				return 0;
		}
		else if(comp==5){
			if(x<=0)
				return 1;
			else
				return 0;
		}
		else
			return -1;
	}
	else if(t=='C'){
		if(comp==0){
			if(a[0]==b[0])
				return 1;
			else
				return 0;
		} 	
		else if(comp==1){
			if(a[0]!=b[0])
				return 1;
			else
				return 0;		
		}
		else if(comp==2){
			if(a[0]>b[0])
				return 1;
			else
				return 0;
		}
		else if(comp==3){
			if(a[0]<b[0])
				return 1;
			else
				return 0;
		}
		else if(comp==4){
			if(a[0]>=b[0])
				return 1;
			else
				return 0;
		}
		else if(comp==5){
			if(a[0]<=b[0])
				return 1;
			else
				return 0;
		}
		else
			return -1;
	}
	else
		return -1;
}

void clauses_clear(struct clauses *str){
	/*if(str!=NULL){
		if(str->str!=NULL)
			;//free(str->str);
		clauses_clear(str->next);
	}*/
}
void clauses_add(struct clauses **str, char *tp1, char *tb1, char *tp2, char *tb2, int op){
	if(*str==NULL){
		*str =  malloc(sizeof(struct clauses));
		(*str)->attr1 = NULL;
		(*str)->tab1 = NULL;
		(*str)->attr2 = NULL;
		(*str)->tab2 = NULL;
	}
	clauses *str2 = *str;
	while(1){
		if(str2->attr1==NULL){
			str2->attr1 = tp1;
			str2->tab1 = tb1;
			str2->attr2 = tp2;
			str2->tab2 = tb2;
			str2->comp = op;
			str2->next = NULL;
			break;	
		}
		else if(str2->next!=NULL)
			str2 = str2->next;
		else{
			str2->next = malloc(sizeof(struct clauses));
			str2->next->attr1 = tp1;
			str2->next->tab1 = tb1;
			str2->next->attr2 = tp2;
			str2->next->tab2 = tb2;
			str2->next->comp = op;
			str2->next->next = NULL;	
			break;
		}
	}
}
int clauses_get(char type[], struct clauses ** claus){
	int x=0, l=0;
	struct clauses * newCl = malloc(sizeof(struct clauses));
	while(type[x]!='\0'){
		if(type[x]=='.'){
			newCl->tab1 = substring(type, l, x-1);
			l = x+1;		
		}
		else if(type[x]=='='||type[x]=='<'||type[x]=='!'||type[x]=='>'){
			newCl->attr1 = substring(type, l,x-1);			
			if(type[x]=='='){
				newCl->comp = 0; 	
				x++;
			} 
			else if(type[x]=='<'){
				if(type[x+1]=='='){
                    newCl->comp = 5;
                    x+=2;                
                }
				else{
                    newCl->comp=3;
                    x++;
                }			
			}
			else if(type[x]=='>'){
				if(type[x+1]=='='){
                    newCl->comp = 4;
                    x+=2;                
                }
				else{
                    newCl->comp = 2;	
                    x++;
                }		
			}
			else if(type[x]=='!'){
				if(type[x+1]=='='){
                    newCl->comp=1;
                    x+=2;                
                }
				else
					return 0;			
			}
            l=x;
            /*Os próximos 3 whiles + 1 if tratam a parte da expressão "xrwer . yrwerw", considerando os espaçamentos
              Essa expressão também pode ser representada sem ponto " yrwerw "  */
            while(isspace(type[x]))
                x++;
            while(type[x]!='\0'&&!isspace(type[x])&&type[x]!='.'&&srcnum(type, x) ==0&&type[x] != '\'')
                x++;
            while(isspace(type[x]))
                x++;
                
            if(srcnum(type, x) == 1){
            	newCl->tab2 = substring("$",0,0);
            	l=x;
            	while(srcnum(type,x))
            		x++;
            	newCl->attr2 = substring(type, l, x-1);
            	while(isspace(type[x]))
                    x++;
            }

            else if(type[x] == '\''){
            	l=++1;
            	while(type[x] != '\'')
            		x++;
            	newCl->tab2 = substring("@",0,0);  //  compara-se atributo com strings, @ significa string 
            	newCl->attr2 = substring(type, l, x-1);
            	x++; //Pula a última apostrofe.
            	while(isspace(type[x]))
                    x++;
	    }
                
            if(type[x]=='.'){
		newCl->tab2 = substring(type, l,x-1);
                l = ++x;				
                while(isspace(type[x]))
                    x++;
                while(type[x]!='\0'&&!isspace(type[x]))
                    x++;
                if(type[x]!='\0'&&x<strlen(type))
                    while(isspace(type[x]))
                        x++;
                newCl->attr2 = substring(type, l, x-1);
            }
            
            
            while(isspace(type[x]))
                        x++;
            if(type[x]=='\0'){
                clauses_add(claus, newCl->attr1, newCl->tab1, newCl->attr2,newCl->tab2, newCl->comp);
                return 1;
            }
            else if(type[x]=='a'||type[x]=='A'){
                if(type[x+1]!='n'&&type[x+1]!='N')
                    return 0;
                else if(type[x+2]!='d'&&type[x+2]!='D')
                    return 0;
                else{
                    newCl->comp+=10;
                       x+=3;                
                }              
            }
            else if(type[x]=='o'||type[x]=='O'){
                if(type[x+1]!='r'&&type[x]!='R')
                    return 0;
                else
                    x+=2;            
            }
            else
                return 0;
            clauses_add(claus, newCl->attr1,newCl->tab1, newCl->attr2,newCl->tab2, newCl->comp);
            while(isspace(type[x]))
                        x++;
            l=x;
		}
		x++; 
    }return 1;
}
int clauses_check(struct clauses *claus, char * table){
    if(claus==NULL){
        printf("NULL!!!!"); return 0;}
	if(!verificaNomeTabela(table)){
            printf("\nERROR: relation \"%s\" was not found.\n\n\n", table);
			return 0;		
		}
    while(1){    
		if(strcmp(claus->tab1, table)!=0){
			printf("The table %s doesn't exist.\n",claus->tab1);
			return 0;		
		}
		if(claus->tab2[0]!='$'&&claus->tab2[0]!='@')
			if(strcmp(claus->tab2, table)!=0){
				printf("The table %s doesn't exist.\n",claus->tab2);	
				return 0;
			}
    	if(claus->next==NULL)
    	    break;
    	else
    	    claus = claus->next;       
    }
    return 1;
}
char * substring(char type[], int from, int to){
	to = (from<to ? to-from : from-to);
	int i;
	char *t = (char*)malloc(sizeof(char*)*(to+2));
	for(i=0;i<=to;i++,from++)
		t[i]=type[from];
    t[i]='\0';
    rmvwhitespaces(&t);
    return t;
	
}

int srcnum(char type[], int pos){ //retorna 1 se char representa numero
	if(type[pos] == '0')
		return 1;
	if(type[pos] == '1')
		return 1;
	if(type[pos] == '2')
		return 1;
	if(type[pos] == '3')
		return 1;
	if(type[pos] == '4')
		return 1;
	if(type[pos] == '5')
		return 1;	
	if(type[pos] == '6')
		return 1;
	if(type[pos] == '7')
		return 1;
	if(type[pos] == '8')
		return 1;
	if(type[pos] == '9')
		return 1;
	return 0;
}

void rmvwhitespaces(char ** type)
{
  char* i = *type;
  char* j = *type;
  while(*j != 0)
  {
    *i = *j++;
    if(!isspace(*i))
      i++;
  }
  *i = 0;
  (*type)[strlen(*type)]='\0';
}
