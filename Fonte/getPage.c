//BufferPool
#include "buffend.h"

// RETORNA PAGINA DO BUFFER
column * getPage(tp_buffer *buffer, tp_table *campos, struct fs_objects objeto, int page){

    if(page >= PAGES)
        return ERRO_PAGINA_INVALIDA;

    if(buffer[page].nrec == 0) //Essa página não possui registros
        return ERRO_PARAMETRO;

    column *colunas = (column *)malloc(sizeof(column)*objeto.qtdCampos*(buffer[page].nrec)); //Aloca a quantidade de campos necessária

    if(!colunas)
        return ERRO_DE_ALOCACAO;

    memset(colunas, 0, sizeof(column)*objeto.qtdCampos*(buffer[page].nrec));

    int i=0, j=0, t=0, h=0;

    if (!buffer[page].position)
        return colunas;

    while(i < buffer[page].position){
        t=0;
        if(j >= objeto.qtdCampos)
            j=0;

        colunas[h].valorCampo = (char *)malloc(sizeof(char)*campos[j].tam+1);
        memset(colunas[h].valorCampo, '\0', campos[j].tam+1);
        colunas[h].tipoCampo = campos[j].tipo;  //Guarda tipo do campo

        strcpy(colunas[h].nomeCampo, campos[j].nome); //Guarda nome do campo

        while(t < campos[j].tam){
            colunas[h].valorCampo[t] = buffer[page].data[i]; //Copia os dados
            t++;
            i++;
        }
        colunas[h].valorCampo[t] = '\0';

        h++;
        j++;
    }

    return colunas; //Retorna a 'page' do buffer
}

column * getOnPageLine(column * page, struct fs_objects *objeto, tp_buffer *bufferpool, char * attr, int n){
	int j;
	for(j=0+(n*objeto->qtdCampos); j < ((n+1)*objeto->qtdCampos); j++)
		if(strcmp(page[j].nomeCampo, attr)==0)
			return &page[j];		
	return NULL;
}

int checkPageLine(column * page1, column *page2, struct fs_objects *objeto1, struct fs_objects *objeto2, tp_buffer *bufferpool1, tp_buffer *bufferpool2, struct clauses * claus, int n){
	column *ret1, *ret2;
	int *aux1, aux2, flagv = 1, flagandor = -1;
	while(claus!=NULL){
		ret1 = getOnPageLine(page1, objeto1, bufferpool1, claus->attr1, n);
		if (ret1 ==NULL);
			ret1 = getOnPageLine(page2, objeto2, bufferpool2, claus->attr1, n);
		if(claus->tab2[0]=='$'){
			aux1 = (int *)ret1->valorCampo;
			aux2 = atoi(claus->attr2);
			
			

			if(claus->comp%10==0){			
				if(flagandor == -1){
					if(*aux1==aux2)
					flagv = 1;
					else flagv = 0;
				}
				else if (flagandor == 0){				
					flagv = flagv||(*aux1==aux2);
				}
				else if (flagandor == 1){
					flagv = flagv && (*aux1==aux2);
				}

			}
			else if(claus->comp%10==1){
				if(flagandor == -1){
					if(*aux1!=aux2)
					flagv = 1;
					else flagv = 0;
				}
				else if (flagandor == 0){				
					flagv = flagv||(*aux1!=aux2);
				}
				else if (flagandor == 1){
					flagv = flagv && (*aux1!=aux2);
				}
			}
			else if(claus->comp%10==2){
				if(flagandor == -1){
					if(*aux1>aux2)
					flagv = 1;
					else flagv = 0;
				}
				else if (flagandor == 0){				
					flagv = flagv||(*aux1>aux2);
				}
				else if (flagandor == 1){
					flagv = flagv && (*aux1>aux2);
				}
			}
			else if(claus->comp%10==3){
				if(flagandor == -1){
					if(*aux1<aux2)
					flagv = 1;
					else flagv = 0;
				}
				else if (flagandor == 0){				
					flagv = flagv||(*aux1<aux2);
				}
				else if (flagandor == 1){
					flagv = flagv && (*aux1<aux2);
				}
				
			}
			else if(claus->comp%10==4){
				if(flagandor == -1){
					if(*aux1>=aux2)
					flagv = 1;
					else flagv = 0;
				}
				else if (flagandor == 0){				
					flagv = flagv||(*aux1>=aux2);
				}
				else if (flagandor == 1){
					flagv = flagv && (*aux1>=aux2);
				}
			}
			else if(claus->comp%10==5){
				if(flagandor == -1){
					if(*aux1<=aux2)
					flagv = 1;
					else flagv = 0;
				}
				else if (flagandor == 0){				
					flagv = flagv||(*aux1<=aux2);
				}
				else if (flagandor == 1){
					flagv = flagv && (*aux1<=aux2);
				}
			}

		}
		else if(claus->tab2[0]=='@'){
			if (flagandor == -1)
				flagv = generalCmp(ret1->valorCampo, claus->attr2, 'S', claus->comp);
			else if (flagandor == 0) 
				flagv = flagv || generalCmp(ret1->valorCampo, claus->attr2, 'S', claus->comp);
			else if (flagandor == 1)
				flagv = flagv && generalCmp(ret1->valorCampo, claus->attr2, 'S', claus->comp);

		}
		else{
			ret2 = getOnPageLine(page1, objeto1, bufferpool1, claus->attr2, n);
			if(ret2==NULL)
				ret2 = getOnPageLine(page2, objeto2, bufferpool2, claus->attr2, n);
			if(ret1==NULL||ret2==NULL){
				return 0; 
			}
			if((ret1->tipoCampo=='D'&&ret2->tipoCampo=='D')||(ret1->tipoCampo=='I'&&ret2->tipoCampo=='D')||(ret1->tipoCampo=='D'&&ret2->tipoCampo=='I')){
				
				if (flagandor == -1)
					flagv = generalCmp(ret1->valorCampo, ret2->valorCampo, 'D', claus->comp);
				else if (flagandor == 0) 
					flagv = flagv || generalCmp(ret1->valorCampo, ret2->valorCampo, 'D', claus->comp);
				else if (flagandor == 1)
					flagv = flagv && generalCmp(ret1->valorCampo, ret2->valorCampo, 'D', claus->comp);		
			}
			else if(ret1->tipoCampo=='I'&&ret2->tipoCampo=='I'){
				
				if (flagandor == -1)
					flagv = generalCmp(ret1->valorCampo, ret2->valorCampo, 'I', claus->comp);
				else if (flagandor == 0) 
					flagv = flagv || generalCmp(ret1->valorCampo, ret2->valorCampo, 'I', claus->comp);
				else if (flagandor == 1)
					flagv = flagv && generalCmp(ret1->valorCampo, ret2->valorCampo, 'I', claus->comp);	
			}
			else if(ret1->tipoCampo=='C'&&ret2->tipoCampo=='C'){
				if (flagandor == -1)
					flagv = generalCmp(ret1->valorCampo, ret2->valorCampo, 'C', claus->comp);
				else if (flagandor == 0) 
					flagv = flagv || generalCmp(ret1->valorCampo, ret2->valorCampo, 'C', claus->comp);
				else if (flagandor == 1)
					flagv = flagv && generalCmp(ret1->valorCampo, ret2->valorCampo, 'C', claus->comp);
			}
			else if(ret1->tipoCampo=='S'&&ret2->tipoCampo=='S'){
				if (flagandor == -1)
					flagv = generalCmp(ret1->valorCampo, ret2->valorCampo, 'S', claus->comp);
				else if (flagandor == 0) 
					flagv = flagv || generalCmp(ret1->valorCampo, ret2->valorCampo, 'S', claus->comp);
				else if (flagandor == 1)
					flagv = flagv && generalCmp(ret1->valorCampo, ret2->valorCampo, 'S', claus->comp);
			}
			else{
				printf("The attributes '%s'%s and '%s'%s are incompatible", ret1->nomeCampo,(ret2->tipoCampo=='C'? "char" : (ret2->tipoCampo=='I'? "integer":(ret2->tipoCampo=='D'? "double":(ret2->tipoCampo=='S'? "string" : "unknown")))), ret2->nomeCampo, (ret2->tipoCampo=='C'? "char" : (ret2->tipoCampo=='I'? "integer":(ret2->tipoCampo=='D'? "double":(ret2->tipoCampo=='S'? "string" : "unknown")))));			
				return 0;		
			}
		}
		if (claus->comp >= 10) // 1 é and 0 é or
			flagandor = 1;
		else 
			flagandor = 0;
		claus = claus->next;
	}
	return flagv;
}
