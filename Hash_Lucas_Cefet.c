#include <stdio.h>
#include <string.h>
#include <locale.h>

#pragma pack(1)

void preenche(int , FILE);

int main(int argc, char** argv){

	setlocale(LC_ALL, "Portuguese");

	FILE *entrada, *s;
    	
	entrada = fopen("cep.dat","r");
	if(!entrada)
	{
		fprintf(stderr,"Arquivo cep.dat não pode ser aberto para leitura\n");
		return 1;
	}

	s = fopen("hash.dat","w+");
	if(!s)
	{
		fclose(entrada);
		fprintf(stderr,"Arquivo %s não pode ser aberto para escrita\n", "hash.dat");
		return 1;
	}

	fseek(entrada,0,SEEK_END);
	
	char c[8];
	char cep, testeProx;
	int   aux, cont, pos, tamanho_hash =   702139 , colisao = 0, max= 0;
	long GuardaPosicao, Proximo, SaidaCEP, Linha = 0, EntradaCEP, ChaveCEP, p, i = 0, chaveColisao, final = ftell(entrada)/300;

	fseek(entrada,0,SEEK_SET);				// Para retornar ao ponto inicial.

	for(i=0;i<tamanho_hash;i++){// iniciando tabela com 0's	
		fprintf(s,"%s","e00000000");	
		fputc(' ',s);                 	
		fprintf(s,"%s","e00000000");
		fputc('\n',s);	
	}
	
	for(i=0;i<final;i++){
		fseek(entrada,290,SEEK_CUR);
		fscanf(entrada, "%08ld", &EntradaCEP);
		fseek(entrada,2,SEEK_CUR);
		
		// Hash (Resto da divisão)
		ChaveCEP = EntradaCEP%tamanho_hash;			
		p = (ChaveCEP*21)-21;			// -21 para compensar a ultima linha em branco
		
		fseek(s,p,SEEK_SET);  	// Vai para a linha para gravar cep

		fread(&cep,1,1,s);    		// Lê o sinal, 'e' para vazio e '*' para ocupado.
        fscanf(s,"%08ld",&SaidaCEP);   
		fseek(s,1,SEEK_CUR);         	//espaco em branco
        fread(&testeProx,1,1,s);     
        fscanf(s,"%08ld",&Proximo);  			
        fseek(s,1,SEEK_CUR);         	// Pula o \n             
		
		if(cep == 'e'){           	// se o campo estiver vazio
            fseek(s,p,SEEK_SET);       // Volta ao inicio da linha.
            fputc('*',s);
            fprintf(s,"%08ld",EntradaCEP);			// Coloca o cep capturado em cep.dat
		}
		if(cep == '*')
			{
            colisao = 0;
			if(testeProx == 'e')
			{
				fseek(s,0,SEEK_END);	// Vai para a ultima linha do arquivo
				Linha = (ftell(s)/21)+1;	// Grava a linha na qual o item foi gravado , para orientação do hash de 'busca'(implementação futura)
            	fputc('*',s);
            	fprintf(s,"%08ld",EntradaCEP);				
							fputc(' ',s);
							fprintf(s,"%s","e00000000");
							fputc('\n',s);
				
				fseek(s,p+10,SEEK_SET);	// Retorna para gravar o local onde o novo CEP foi armazenado
				fputc('*',s);
                sprintf(c,"%08ld",Linha);    // de long para char
                fwrite(c,8,1,s);					
								fputc('\n',s);
			}	
            if(testeProx == '*'){
            		while(testeProx == '*')
								{		
									colisao++;
									if(colisao>max)
									{	
										max = colisao;
										chaveColisao = ChaveCEP;
									}
						
						GuardaPosicao = Proximo;
						fseek(s,((Proximo*21)-21)+10,SEEK_SET);	// vai para vai para a linha do informada no campo "próximo"
						fread(&testeProx,1,1,s);
						fscanf(s,"%08ld",&Proximo);
						
						if(testeProx == 'e'){
							
							fseek(s,0,SEEK_END);			// Vai para a ultima linha do arquivo
							Linha = (ftell(s)/21)+1;			// Grava a linha na qual o proximo item foi gravado
							fputc('*',s);
				            fprintf(s,"%08ld",EntradaCEP);			
							fputc(' ',s);
							fprintf(s,"%s","e00000000");
							fputc('\n',s);
							
							fseek(s,((GuardaPosicao*21)-21)+9,SEEK_SET);	// Retorna para gravar o local onde o novo CEP foi armazenado
							fputc(' ',s);
							fputc('*',s);
				            sprintf(c,"%08ld",Linha); 
				            fwrite(c,8,1,s);					
							fputc('\n',s);
						}
          			}
       			}
			}
		}
	printf("Maior sequência de colisões: %i com chave: %ld\n", max, chaveColisao);
	
	fclose(entrada);
	fclose(s);
	
	system("pause");
}

