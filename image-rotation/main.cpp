#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include "image_processing.cpp"

using namespace std;

void sagaDondur(short n, int resim);
void solaDondur(short n, int resim);

int main(void) {
	int M, N, Q, i, j, k;
	bool type;
	int efile, islem;
	char resimadi[100], sonek[10];
	printf("CANER KAYA 16011087\n");
	do {
		printf("Islem yapilacak resmin yolunu (path) giriniz:\n-> ");
		scanf("%s", &resimadi);
		system("CLS");
		efile = readImageHeader(resimadi, N, M, Q, type);
	} while (efile > 1);
	printf("%s\n", resimadi);
	int** resim = resimOku(resimadi);

	short *resimdizi;
	resimdizi = (short*) malloc(N*M * sizeof(short));

	for (i = 0; i < N; i++) 
		for (j = 0; j < M; j++) 
			resimdizi[i*N + j] = (short) resim[i][j];

	int resimadres = (int) resimdizi;

	do {
		system("CLS");
		printf("\t     ISLEMLER\n");
		printf("------------------------------------\n");
		printf("1)  Resmi saga dondur\n");
		printf("2)  Resmi sola dondur\n");
		printf("0)  Cikis\n\n");
		printf("\"%s\" yolundaki resim icin yapilacak islemi seciniz\n-> ", resimadi);
		scanf("%d", &islem);
	} while (islem > 2 || islem < 0);

	switch (islem) {
		case 0:
			exit(0);
		case 1:
			sagaDondur(N, resimadres);
			strcpy(sonek, "_sag.pgm");
			break;
		case 2:
			solaDondur(N , resimadres);
			strcpy(sonek, "_sol.pgm");
			break;
		default:
			strcpy(sonek, "_orj.pgm");
			break;
	}

	for (k = 0; k < N * M; k++) {
		j = k % N;
		i = k / N;
		resim[i][j] = (int)resimdizi[k];
	}
	
	string::size_type pos = string(resimadi).find_last_of(".");
	resimadi[pos] = '\0';
	strcat(resimadi, sonek);
	resimYaz(resimadi, resim, N, M, Q);
	printf("\nIslem basariyla tamamlandi :)\n\"%s\" yolunda resim olusturuldu.\n\n", resimadi);
	system("PAUSE");
	return 0;
}

void sagaDondur(short n, int resim) {
	__asm {
	
	//dis dongu tur sayisi hesaplanmasi
	MOV AX,n 
	SHR AX,1
	DEC AX
	MOV CX,AX  //cx =(n/2)-1
	
	MOV ESI,0   //SI dis dongunun counterý olarak kullanilacak, matrisin i satir indisini temsil ediyor 
L1: PUSH CX //cx degeri stackte saklanmalý nested loop var	
	//ic dongu cevrim sayisi hesaplanmasi
	MOV AX,n 
	SHR AX,1
	INC AX
	MOV CX,AX //cx=(n/2)+1
	
L2: 
	 //matrisin sutun indisini temsil edecek j degerinin CX uzerinden hesaplanmasi
	 MOV AX,n
	 shr AX,1
	 INC AX	
	 SUB AX,cx //AX= (n/2)+1-CX  yani j ilk tur icin 0 dan baslayip birer birer artacak
	 PUSH AX //lazim oldugunda stackten okunacak
	 
	//[i*n+j] elde edilmesi
	 XOR EAX,EAX    //AX in extended bitleri sifirlandi  
	 MOV AX,n           
	 MUL ESI       //carpmanin sonucu EAXde, tasma ihtimali yok
	 XOR EDX,EDX     
	 PUSH EBP       //EBP degeri korunuyor
	 MOV EBP,ESP    //EBP stacki gösteriyor
	 MOV DX,[EBP+4] //j degeri stackin 4byte gerisindeydi
	 POP EBP        //EBP degeri korundu
	 ADD EAX,EDX    //EAX=i*n+j ,AX max 65536 degerini aldigi icin EAX kullanmak onemli
	 SHL EAX,1      //matris WORD tipi oldugu icin 2 ile carpiyoruz
	 MOV EDI,resim
	 ADD EDI,EAX    //[EDI]=RESIM[[i*n+j]
 
	 MOV AX,WORD PTR[EDI]
	 PUSH AX  // TMP degeri olarak saklanacak
		   
	 //[n*(n-1-j)+i] ELDE EDILMESI    
	 XOR EAX,EAX    //AX in extended bitleri sifirlandi
	 MOV AX,n         
	 DEC EAX		
	 PUSH EBP       
	 MOV EBP,ESP      //stackten j degeri cekilecek
	 MOV DX,[EBP+6]	  //yukarýda TMP pushlandigi icin 2byte daha altta kalmisti
	 POP EBP          
	 SUB AX,DX        
	 XOR EDX,EDX      //DX: word ->dword atamalarinda ara degisken gorevi goruyor  
	 MOV DX,n
	 MUL EDX         
	 ADD EAX,ESI
	 SHL EAX,1        //matris WORD tipi oldugu icin 2 ile carpiyoruz
	 MOV EBX,resim
	 ADD eBX,EAX      //[BX]=RESIM[n*(n-1-j)+i]  
	 
	 MOV AX,WORD PTR [EBX]
	 MOV WORD PTR [EDI],AX //RESIM[i*n+j]=RESIM[n*(n-1-j)+i]  
	 
	 //LOOP UZUN OLDUGU ICIN BASA DONMESI SAGLANDI
	 JMP gec
	 L22: JMP L2
	 L11: JMP L1
	 gec:

	 //[n*(n-1-i)+n-1-j] ELDE EDILMESI
	 XOR EAX,EAX 
	 MOV AX,n
	 DEC EAX
	 SUB AX,SI
	 XOR EDX,EDX
	 MOV DX,n
	 MUL EDX
	 XOR EDX,EDX
	 MOV DX,n //AX taþýnca sýfýrlanýyor EAX e eklemek lazým
	 ADD EAX,EDX
	 DEC EAX
	 PUSH EBP
	 MOV EBP,ESP
	 MOV DX,[EBP+6]
	 POP EBP
	 SUB AX,DX
	 SHL EAX,1  //matris WORD tipi oldugu icin 2 ile carpiyoruz    
	 MOV EDI,resim
	 ADD EDI,EAX  //[EDI]=RESIM[n*(n-1-i)+n-1-j]
	 
	 MOV AX,WORD PTR[EDI]
	 MOV WORD PTR [EBX],AX //a[n*(n-1-j)+i]=a[n*(n-1-i)+n-1-j]
	 
	 //[j*n+n-1-i] ELDE EDILMESI
	 PUSH EBP
	 MOV EBP,ESP
	 MOV DX,[EBP+6]  //stackten j degeri cekildi
	 POP EBP
	 XOR EAX,EAX
	 MOV AX,DX
	 XOR EDX,EDX
	 MOV DX,n
	 MUL EDX
	 XOR EDX,EDX  
	 MOV DX,n
	 ADD EAX,EDX
	 DEC EAX
	 SUB AX,SI 
	 SHL EAX,1  //matris WORD tipi oldugu icin 2 ile carpiyoruz  
	 MOV EBX,resim  
	 ADD EBX,EAX  //[EBX]=RESIM[j*n+n-1-i] 
	
	 MOV AX,WORD PTR [EBX]
	 MOV WORD PTR [EDI],AX //RESIM[n*(n-1-i)+n-1-j]=RESIM[j*n+n-1-i]
	 
	 POP AX   //TMP degerý stackten cikarildi
	 MOV WORD PTR[EBX],AX  //RESIM[j*n+n-1-i]=TMP
			 		
	 POP DX	 //j degeri bu tur icin kullanildi artik stackten cikarilabilir
	 LOOP L22
	 POP CX  //dis dongu tur sayisi
	 INC SI  //i++
	 LOOP L11
   }
}

void solaDondur(short n, int resim) {
	__asm {
    //dis dongu tur sayisi hesaplanmasi
	MOV AX,n
	SHR AX,1
	DEC AX
	MOV CX,AX  //cx =(n/2)-1
	
	MOV ESI,0 //SI dis dongunun counterý olarak kullanilacak, matrisin i satir indisini temsil ediyor 
L1: PUSH CX   //cx degeri stackte saklanmalý nested loop var	
    //ic dongu tur sayisi hesaplanmasi
	MOV AX,n
	SHR AX,1
	INC AX
	MOV CX,AX  //cx=(n/2)+1	
L2: 
	 //matrisin sutun indisini temsil edecek j degerinin CX uzerinden hesaplanmasi
	 MOV AX,n
	 SHR AX,1
	 INC AX	
	 SUB AX,cx //AX= (n/2)+1-CX  yani j ilk tur icin 0 dan baslayip birer birer artacak
	 PUSH AX   //lazim oldugunda stackten okunacak
	 
	//[i*n+j] ELDE EDILMESI 
	 XOR EAX,EAX   //AX in extended bitleri sifirlandi 
	 MOV AX,n
	 MUL ESI //carpmanin sonucu EAXde, tasma ihtimali yok
	 XOR EDX,EDX     
	 PUSH EBP       //EBP degeri korunuyor
	 MOV EBP,ESP    //EBP stacki gösteriyor
	 MOV DX,[EBP+4] //j degeri stackin 4byte gerisindeydi
	 POP EBP        //EBP degeri korundu
	 ADD EAX,EDX    //EAX=i*n+j ,AX max 65536 degerini aldigi icin EAX kullanmak onemli
	 SHL EAX,1      //matris WORD tipi oldugu icin 2 ile carpiyoruz
	 MOV EDI,resim
	 ADD EDI,EAX    //[EDI]=RESIM[[i*n+j] 
	 
	 MOV AX,WORD PTR[EDI]
	 PUSH AX  // TMP stackte gomuluyor
	 
	 //[j*n+n-1-i] ELDE EDILMESI	
	 PUSH EBP
	 MOV EBP,ESP
	 MOV DX,[EBP+6]  //stackten j degeri cekildi
	 POP EBP
	 XOR EAX,EAX
	 MOV AX,DX
	 XOR EDX,EDX
	 MOV DX,n
	 MUL EDX
	 XOR EDX,EDX  
	 MOV DX,n
	 ADD EAX,EDX
	 DEC EAX
	 SUB AX,SI 
	 SHL EAX,1     //matris WORD tipi oldugu icin 2 ile carpiyoruz 
	 MOV EBX,resim  
	 ADD EBX,EAX   //[EBX]=RESIM[j*n+n-1-i] 

	 MOV AX,WORD PTR [EBX]
	 MOV WORD PTR [EDI],AX   //RESIM[[i*n+j]=RESIM[j*n+n-1-i]
		

	 //[n*(n-1-i)+n-1-j] ELDESI
	 XOR EAX,EAX
	 MOV AX,n
	 DEC EAX
	 SUB AX,SI
	 XOR EDX,EDX
	 MOV DX,n
	 MUL EDX
	 XOR EDX,EDX
	 MOV DX,n //AX taþýnca sýfýrlanýyor eklemeleri EAX e yapmak lazým
	 ADD EAX,EDX
	 DEC EAX
	 PUSH EBP
	 MOV EBP,ESP
	 MOV DX,[EBP+6]
	 POP EBP
	 SUB AX,DX
	 SHL EAX,1  //matris WORD tipi oldugu icin 2 ile carpiyoruz    
	 MOV EDI,resim
	 ADD EDI,EAX  //[EDI]=RESIM[n*(n-1-i)+n-1-j]
	 
	 MOV AX,WORD PTR[EDI]
	 MOV WORD PTR [EBX],AX //a[n*(n-1-j)+i]=a[n*(n-1-i)+n-1-j]

	 //[n*(n-1-j)+i] ELDE EDILMESI    
	 XOR EAX,EAX
	 MOV AX,n
	 DEC EAX
	 PUSH EBP
	 MOV EBP,ESP
	 MOV DX,[EBP+6]
	 POP EBP
	 SUB AX,DX
	 XOR EDX,EDX
	 MOV DX,n
	 MUL EDX
	 ADD EAX,ESI
	 SHL EAX,1  //WORD TIPI
	 MOV EBX,resim
	 ADD EBX,EAX  //[EBX]=RESIM[n*(n-1-j)+i]  
	 
	 MOV AX,WORD PTR [EBX]
	 MOV WORD PTR [EDI],AX //RESIM[I*N+J]=RESIM[n*(n-1-j)+i]
	
	 POP AX   //TMP degerý stackten cikarildi
	 MOV WORD PTR [EBX],AX  //RESIM[n*(n-1-j)+i]=TMP

	 //LOOP UZUN OLDUGU ICIN JMP ILE BASA DONMESI SAGLANDI
	 JMP gec
	 L22: JMP L2
	 L11: JMP L1
	 gec:	
	 
	 POP DX   //j degeri bu tur icin kullanildi artik stackten cikarilabilir
	 LOOP L22
	 POP CX  //dis dongu tur sayisi
	 INC SI  //i++
	 LOOP L11
 }
}	