//BufferPool
#include "buffend.h"
#include <ctype.h>

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Utilizada para impressão de tabelas (select * from table;).
    Parametros: Nome da tabela (char).
    Retorno:    void.
   ---------------------------------------------------------------------------------------------*/
void imprimese(char nomeTabela[], char type[], char projecao[], char joinTab[], char joinCond[]){
	int j,erro, x, p, cont=0, n;
    struct fs_objects objeto;
    struct clauses *constr = NULL;
    clauses_get(type, &constr);
    int whereCl = clauses_check(constr, nomeTabela);/* (-1) Indica não presença da cláusula where */
    projCampos * campo;
    campo = NULL;

    if(projecao!=NULL)
    	campo = setProjAttr(projecao);


    if(whereCl==-1||whereCl==1){
	
        if(!verificaNomeTabela(nomeTabela)){
            printf("\nERROR: relation \"%s\" was not found.\n\n\n", nomeTabela);
            return;
        }

    	objeto = leObjeto(nomeTabela);

    	tp_table *esquema = leSchema(objeto);

    	if(esquema == ERRO_ABRIR_ESQUEMA){
    	    printf("ERROR: schema cannot be created.\n");
    	    free(esquema);
    	    return;
    	}

    	tp_buffer *bufferpoll = initbuffer();

    	if(bufferpoll == ERRO_DE_ALOCACAO){
    	    free(bufferpoll);
    	    free(esquema);
    	    printf("ERROR: no memory available to allocate buffer.\n");
    	    return;
    	}

    	erro = SUCCESS;
    	for(x = 0; erro == SUCCESS; x++)
    	    erro = colocaTuplaBuffer(bufferpoll, x, esquema, objeto);

    	int ntuples = --x;
        ntuples*=objeto.qtdCampos;
		p = 0;
		n=-1;
		int chk;
		while(x){
		    column *pagina = getPage(bufferpoll, esquema, objeto, p);
		    if(pagina == ERRO_PARAMETRO){
    	        printf("ERROR: could not open the table.\n");
    	        free(bufferpoll);
    	        free(esquema);
    	        return;
		    }

		    if(!cont) {
		        for(j=0; j < objeto.qtdCampos; j++){ //checkProj(char tabela[], char atributo[], projCampos * campo[])
			        if(checkProj(nomeTabela, pagina[j].nomeCampo, campo)){  
			            if(pagina[j].tipoCampo == 'S')
			                printf(" %-20s ", pagina[j].nomeCampo);
			        	else
			                printf(" %-10s ", pagina[j].nomeCampo);
			            if(j<objeto.qtdCampos-1)
			            	printf("|");
			        }
		        }
		        printf("\n");
		        for(j=0; j < objeto.qtdCampos; j++){
		            if(checkProj(nomeTabela, pagina[j].nomeCampo, campo)){
		            	printf("%s",(pagina[j].tipoCampo == 'S')? "----------------------": "------------");
		            	if(j<objeto.qtdCampos-1)
		            		printf("+");
		            }
		        }
		        printf("\n");
		    }
		    cont++;       
    
			for(j=0; j < objeto.qtdCampos*bufferpoll[p].nrec; j++){
	        	if(n<(int)j/objeto.qtdCampos){
					n++;
					if(whereCl==-1)
                                        	chk=1;
                                        else 
						chk = checkPageLine(pagina, &objeto, bufferpoll, constr,n);
				}
				if(chk && checkProj(nomeTabela, pagina[j].nomeCampo, campo)){ 
					if(pagina[j].tipoCampo == 'S')
		 	           	printf(" %-20s ", pagina[j].valorCampo);
		        	else if(pagina[j].tipoCampo == 'I'){
		            	int *n = (int *)&pagina[j].valorCampo[0];
		            	printf(" %-10d ", *n);
		        	}
					else if(pagina[j].tipoCampo == 'C'){
		            	printf(" %-10c ", pagina[j].valorCampo[0]);
		        	} 
					else if(pagina[j].tipoCampo == 'D'){
		            	double *n = (double *)&pagina[j].valorCampo[0];
		    	        printf(" %-10f ", *n);
		        	}
	            	
				}
				

                if(chk){
					if(j>=1 && ((j+1)%objeto.qtdCampos)==0)
	            			printf("\n");
	        			else if (checkProj(nomeTabela, pagina[j].nomeCampo, campo))
	        				printf("|");
	        	}else
                	ntuples--;

				
    		}
			x-=bufferpoll[p++].nrec;
		}
	ntuples/=objeto.qtdCampos;
    	printf("\n(%d %s)\n\n",ntuples,(1>=ntuples)?"row": "rows");
		free(bufferpoll);
	    free(esquema);
	}
}
projCampos *setProjAttr(char projecao[]){ //joga campos na estrutura de campos da projeção
	projCampos * strcampo, *strcamphead;
	int x=0, l=0, flag=0;

	strcampo = (projCampos*) malloc(sizeof(strcampo));
	strcampo->next = NULL;
	strcamphead = strcampo;

	while(projecao[x]!='\0'){
		
		while(isspace(projecao[x])){
		    x++; flag=1;
		}
		if (flag==1)
			l=x;
		flag = 0;

		while(projecao[x]!='.')
			x++;
		strcampo->tabela = substring(projecao, l,x-1,1);
		l=x+1;

		while(projecao[x]!=',' && projecao[x]!='\0')
			x++;

		
		strcampo->campo = substring(projecao, l,x-1,1);
		if(projecao[x]==','){
			strcampo->next = (projCampos*) malloc(sizeof(strcampo));
			strcampo = strcampo->next;
			strcampo->next = NULL;
		}
		l= ++x;
		

	}


	return strcamphead;

}

int checkProj(char tabela[], char atributo[], projCampos * campo){
	if (campo == NULL )
		return 1;
	while(campo!=NULL){
		if(strcmp(tabela,campo->tabela)==0 && strcmp(atributo,campo->campo)==0){
			return 1; 
		}
		campo = campo->next;
	}
	return 0;
}
