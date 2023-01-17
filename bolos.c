#define __USE_POSIX
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>

/* author: Carlos Rodríguez.
   GitHub: crr004. */


//SOBRE EL PROGRAMA:
/*********************************************************************************************************************************************
*  El programa bolos crea un árbol de procesos tal y como se indica en el enunciado de la práctica.                                          *
*  Se debe ejecutar con ./bolos y después hacer ps -fu para ver el PID de A.                                                                 *
*  Mandaremos por terminal un kill -SIGTERM <PID de A> para realizar una tirada.                                                             *
*  La señal se irá propagando de manera aleatoria y A imprimirá en forma de pirámide el estado de los bolos.                                 *
*  Se ejecutará de manera automática un ps -fu para comprobar que lo impreso coincide con la realidad.                                       *
*  A matará a los bolos que queden vivos y el programa finalizará.                                                                           *
*  Se ha hecho un control exhaustivo de errores en las llamadas al sistema.                                                                  *
*  El control de errores se realiza en la misma línea de código que la llamada cuyos errores controla para mejorar la legibilidad del código.*
*********************************************************************************************************************************************/

//Función que usamos como manejadora para cuando un proceso recibe SIGTERM.
void aux(int s){
    return;
}

//Función para saber cuál de las cuatro opciones de "asesinato" debe realizar el proceso.
int tiempo(){	
	struct timeval time;
	int result;
	
	gettimeofday(&time, NULL);
	
	result = time.tv_usec % 4;

	if(result == 0)
		return 1;
	else if (result == 1)
		return 2;
	else if (result == 2)
		return 3;
	else if (result == 3)
		return 4;

}

int main(int argc, char *argv[]){
        int valordevuelto, valordevuelto2, id, decision, HhaMuerto, EhaMuerto, IhaMuerto, vivosIzqda, vivosDrcha, devolver; //Variables que se usarán a lo largo del programa.
        int meComunicoConA[5]; //Pid's de B, C, H, E, I respectivamente.
        int meComunicoConB[2]; //Pid's de D y E respectivamente.
        int meComunicoConC[2]; //Pid's de E y F respectivamente.
        int meComunicoConD[2]; //Pid's de G y H respectivamente.
        int meComunicoConE[2]; //Pid's de H e I respectivamente.
        int meComunicoConF[2]; //Pid's de I y J respectivamente.
        
        //Declaración de máscaras y sigaction.
	struct sigaction ss;
	sigset_t maskterm, mask;
	
	//Esta máscara es la que tendrán los procesos durante todo el programa menos cuando sigsuspend la cambia por la de abajo.
	if(sigfillset(&mask) == -1){ perror("Error sigfillset"); return -1;}
	if(sigdelset(&mask, SIGINT) == -1){ perror("Error sigdelset"); return -1;}
	if(sigprocmask(SIG_SETMASK, &mask, NULL) == -1){ perror("Error sigprocmask"); return -1;}
	
	//Esta es la máscara que establece sigsuspend y la que usamos para el sigaction de SIGTERM.
	if(sigfillset(&maskterm) == -1){ perror("Error sigfillset"); return -1;}
	if(sigdelset(&maskterm,SIGTERM) == -1){ perror("Error sigdelset"); return -1;}
	if(sigdelset(&maskterm, SIGINT) == -1){ perror("Error sigdelset"); return -1;}
	ss.sa_handler=&aux;
        ss.sa_flags=SA_RESTART;
	
	if(sigaction(SIGTERM,&ss,NULL) == -1){ perror("Error sigaction"); return -1;}

	//Comienzo de la creación del árbol de procesos: P crea a A.	
	if(!strcmp(argv[0],"./bolos")){
	    id = fork();
	    switch(id){
	        case -1:perror("Error forkP");
	                return 1;
	        case 0:if(execl(argv[0],"A",NULL) == -1){ perror("Error execl de A"); return -1;} //exec de A.    
	               exit(0);     	                 	   
	       default: //Proceso P.	   	    
	    	        exit(0);
	    }
	}
           
        //Por aquí solo entra el proceso A. Crea sus 5 hijos y almacena sus PID's,
	if(!strcmp(argv[0], "A")){
	    char lePasoHaByE[10], lePasoEaByC[10], lePasoIaCyE[10];
	    
	    id=fork();
	    switch(id){ //Proceso I.
	        case -1:perror("Error forkI"); 
	                return -1;
	        case 0:if(execl("./bolos","I",NULL) == -1){ perror("Error execl de I"); return -1;} //exec de I.
	        default:meComunicoConA[4]=id;	
	    }
            
            id=fork();  
	    switch(id){ //Proceso H.
	        case -1:perror("Error forkH");
	    	        return -1;
	        case 0:if(execl("./bolos","H",NULL) == -1){ perror("Error execl de H"); return-1;} //exec de H.
	        default:meComunicoConA[2]=id;	
	    }
	    
	    id=fork();
	    switch(id){ //Proceso E.
	        case -1:perror("Error forkE");
	    	        return -1;
	        case 0:sprintf(lePasoHaByE,"%d", meComunicoConA[2]); //meComunicoConA[2] = PID de H
	               sprintf(lePasoIaCyE,"%d", meComunicoConA[4]); //meComunicoConA[4] = PID de I
	               if(execl("./bolos", "E", lePasoHaByE, lePasoIaCyE, NULL) == -1){ perror("Error execl de E"); return -1;} //exec de E.
	        default:meComunicoConA[3]=id;	
	    }
	    
	    id=fork();
	    switch(id){ //Proceso B.
	        case -1:perror("Error forkB");
	    	        return -1;
	        case 0:sprintf(lePasoHaByE,"%d", meComunicoConA[2]); //meComunicoConA[2] = PID de H
	               sprintf(lePasoEaByC,"%d", meComunicoConA[3]); //meComunicoConA[3] = PID de E
	               if(execl("./bolos","B", lePasoHaByE, lePasoEaByC, NULL) == -1){ perror("Error execl de B"); return -1;} //exec de B.
	        default:meComunicoConA[0]=id;
	    }
	    
	    id=fork();
	    switch(id){ //Proceso C.
	        case -1:perror("Error forkC");
	    	        return -1;
	        case 0:sprintf(lePasoIaCyE,"%d", meComunicoConA[4]); //meComunicoConA[4] = PID de I
	               sprintf(lePasoEaByC,"%d", meComunicoConA[3]); //meComunicoConA[3] = PID de E
	               if(execl("./bolos", "C", lePasoEaByC, lePasoIaCyE, NULL) == -1){ perror("Error execl de C"); return -1;} //exec de C.
	        default:meComunicoConA[1]=id;
	    }
	}
	
	//Por aquí solo entra el proceso B. Crea a D y almacena su PID y el de E (que se lo ha pasado A).
	if(!strcmp(argv[0], "B")){
	    char lePasoHaD[10];
	    
	    meComunicoConB[1]=atoi(argv[2]);
	    
	    id=fork();
	    switch(id){ //Proceso D.
	        case -1:perror("Error forkD");
	                return -1; 
	        case 0:sprintf(lePasoHaD,"%s", argv[1]);
	               if(execl("./bolos","D", lePasoHaD,NULL) == -1){ perror("Error execl de D"); return -1;} //exec de D.
	        default:meComunicoConB[0]=id;
	    }
	}
	
	//Por aquí solo entra el proceso C. Crea F y almacena su PID y el de E (que se lo ha pasado A).
	if(!strcmp(argv[0], "C")){ 
	    char lePasoIaF[10];
	    
	    meComunicoConC[0]=atoi(argv[1]);
	    
	    id=fork();
	    switch(id){ //Proceso F.
	        case -1:perror("Error forkF");
	                return -1; 
	        case 0:sprintf(lePasoIaF,"%s", argv[2]);
	               if(execl("./bolos", "F", lePasoIaF, NULL) == -1){ perror("Error execl de F"); return -1;} //exec de F.
	        default:meComunicoConC[1]=id;
	    }  
	}
	
	//Por aquí solo entra el proceso D. Crea G y almacena su PID y el de H (que se lo ha pasado B, que lo recibió de A).
	if(!strcmp(argv[0], "D")){
	    
	    meComunicoConD[1]=atoi(argv[1]);
	    
	    id=fork();
	    switch(id){ //Proceso G.
	        case -1:perror("Error forkG");
	                return -1;
	        case 0:if(execl("./bolos","G",NULL) == -1){ perror("Error execl de G"); return -1;} //exec de G.
	        default:meComunicoConD[0]=id;
	        
	    }
	}
	
	//Por aquí solo entra el proceso E. Almacena los PID's de H e Y, que se los ha pasado A.
	if(!strcmp(argv[0], "E")){
	    meComunicoConE[0]=atoi(argv[1]);
	    meComunicoConE[1]=atoi(argv[2]); 
	}
	
	//Por aquí solo entra el proeso F. Crea J y almacena su PID y el de I (que se lo ha pasado C, que lo recibió de A).
	if(!strcmp(argv[0], "F")){
	    
	    meComunicoConF[0]=atoi(argv[1]);
	    
	    id=fork();
	    switch(id){ //Proceso J.
	        case -1:perror("Error forkJ");
	                return -1;
	        case 0:if(execl("./bolos","J",NULL) == -1){ perror("Error execl de J"); return -1;} //exec de J.
	        default:meComunicoConF[1]=id;
	    }
	}
	
	//Aquí se quedan todos los procesos esperando a recibir SIGTERM o SIGINT.
	sigsuspend(&maskterm);
	
	//Por aquí solo entra el proceso A. Hace lo que el resultado de la función tiempo() le indique, mira si I, H y E siguen vivos,
	//recoge los códigos de retorno de B y C para ver la situación de cada ristra, imprime la situación por pantalla 
	//y finalmente crea un hijo para ejecutar el ps -fu final.
	if(!strcmp(argv[0], "A")){
	    decision = tiempo();
	    switch(decision){
	        case 1: vivosIzqda=3;
	                vivosDrcha=3;
	                break;
	        case 2: if(kill(meComunicoConA[1], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(waitpid(meComunicoConA[1], &valordevuelto, 0) == -1){ perror("Error waitpid"); return -1;}
	                vivosDrcha=WEXITSTATUS(valordevuelto);
	                vivosIzqda=3;
	                break;
	        case 3: if(kill(meComunicoConA[0], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(waitpid(meComunicoConA[0], &valordevuelto, 0) == -1){ perror("Error waitpid"); return -1;}
	                vivosIzqda=WEXITSTATUS(valordevuelto);
	                vivosDrcha=3;
	                break;
	        case 4: if(kill(meComunicoConA[0], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(waitpid(meComunicoConA[0], &valordevuelto, 0) == -1){ perror("Error waitpid"); return -1;}
	                vivosIzqda=WEXITSTATUS(valordevuelto);
	                if(kill(meComunicoConA[1], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(waitpid(meComunicoConA[1], &valordevuelto2, 0) == -1){ perror("Error waitpid"); return -1;}
	                vivosDrcha=WEXITSTATUS(valordevuelto2);
	                break;
	    }
	    
	    sleep(6); //Ponemos 6 segundos para asegurar su funcionamiento en Nogal.
	    if((HhaMuerto = waitpid(meComunicoConA[2], &valordevuelto, WNOHANG)) == -1){ perror("Error waitpid"); return -1;} //Comprueba si H sigue vivo.
	    if((EhaMuerto = waitpid(meComunicoConA[3], &valordevuelto, WNOHANG)) == -1){ perror("Error waitpid"); return -1;} //Comprueba si E sigue vivo.
	    if((IhaMuerto = waitpid(meComunicoConA[4], &valordevuelto, WNOHANG)) == -1){ perror("Error waitpid"); return -1;} //Comprueba si I sigue vivo.
	    
	    //Se imprime el estado de los bolos tras propagar las señales oportunas:
	    char buffer[100];
	    
	    sprintf(buffer, "\n");
            if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}
            sprintf(buffer, "\n                 X                  \n");
	    if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}
	    if(vivosIzqda<3){
	        sprintf(buffer, "           X       ");
	        if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
	    else{
	        sprintf(buffer, "           B       ");
	        if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return-1;}}
	    if(vivosDrcha<3){
	        sprintf(buffer, "    X           \n");
	        if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
	    else{
	        sprintf(buffer, "    C           \n");
	        if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
	    if(vivosIzqda<2){
                sprintf(buffer, "       X    ");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            else{
                sprintf(buffer, "       D    ");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            if(!EhaMuerto){
                sprintf(buffer, "     E       ");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            else{
                sprintf(buffer, "     X       ");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            if(vivosDrcha<2){
                sprintf(buffer, "  X     \n");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            else{
                sprintf(buffer, "  F     \n");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            if(vivosIzqda<1){
                sprintf(buffer, "   X   ");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            else{
                sprintf(buffer, "   G   ");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            if(!HhaMuerto){
                sprintf(buffer, "    H       ");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            else{
                sprintf(buffer, "    X       ");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            if(!IhaMuerto){
                sprintf(buffer, "    I     ");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            else{
                sprintf(buffer, "    X     ");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            if(vivosDrcha<1){
                sprintf(buffer, "  X  \n");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            else{
                sprintf(buffer, "  J  \n");
                if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}}
            sprintf(buffer, "\n");
            if(write(1,buffer,strlen(buffer)) == -1){ perror("Error write"); return -1;}
            
            //Finalmente, A tiene un hijo que ejecuta un ps -fu para comprobar que el resultado de la tirada es correcto.
            id = fork();
            switch(id){
                case -1:perror("Error forkPSdeA");
                        return -1;
                case 0:if(execl("/bin/ps", "ps","-fu", NULL) == -1){ perror("Error exec de PS"); return -1;}
            }        
            if(waitpid(id,&valordevuelto,0) == -1){ perror("Error waitpid"); return -1;}
            if(kill(0,SIGINT) == -1){ perror("Error kill"); return -1;}        
	}
	
	//Por aquí solo entra el proceso B. Hace una cosa u otra según la función tiempo() y devuelve lo necesario a A.
	if(!strcmp(argv[0], "B")){  
	    decision = tiempo();
	    switch(decision){
	        case 1: return 2;
	        case 2: if(kill(meComunicoConB[1], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                return 2;
	        case 3: if(kill(meComunicoConB[0], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(waitpid(meComunicoConB[0], &valordevuelto, 0) == -1){ perror("Error waitpid"); return -1;}
	                return WEXITSTATUS(valordevuelto);
	        case 4: if(kill(meComunicoConB[0], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(waitpid(meComunicoConB[0], &valordevuelto, 0) == -1){ perror("Error waitpid"); return -1;}
	                devolver = WEXITSTATUS(valordevuelto);
	                if(kill(meComunicoConB[1], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                return devolver;
	    } 
	}
	
	//Por aquí solo entra el proceso C. Hace una cosa u otra según la función tiempo() y devuelve lo necesario a A.
	if(!strcmp(argv[0], "C")){ 
	    decision = tiempo();
	    switch(decision){
	        case 1: return 2;
	        case 2: if(kill(meComunicoConC[1], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(waitpid(meComunicoConC[1], &valordevuelto, 0) == -1){ perror("Error waitpid"); return -1;}
	                return WEXITSTATUS(valordevuelto);
	        case 3: if(kill(meComunicoConC[0], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                return 2;
	        case 4: if(kill(meComunicoConC[0], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(kill(meComunicoConC[1], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(waitpid(meComunicoConC[1], &valordevuelto2, 0) == -1){ perror("Error waitpid"); return -1;}
	                devolver = WEXITSTATUS(valordevuelto2);
	                return devolver;
	    }  
	}
	
	//Por aquí solo entra el proceso D. Hace una cosa u otra según la función tiempo() y devuelve lo necesario a B.
	if(!strcmp(argv[0], "D")){   
	    decision = tiempo();
	    switch(decision){
	        case 1: return 1;
	        case 2: if(kill(meComunicoConD[1], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                return 1;
	        case 3: if(kill(meComunicoConD[0], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(waitpid(meComunicoConD[0], &valordevuelto, 0) == -1){ perror("Error waitpid"); return -1;}
	                return 0;
	        case 4: if(kill(meComunicoConD[0], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(waitpid(meComunicoConD[0], &valordevuelto, 0) == -1){ perror("Error waitpid"); return -1;}
	                if(kill(meComunicoConD[1], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                return 0;
	    }
	}
	
	//Por aquí solo entra el proceso E. Hace una cosa u otra según la función tiempo().
	if(!strcmp(argv[0], "E")){   
	    decision = tiempo();
	    switch(decision){
	        case 1: break;
	        case 2: if(kill(meComunicoConE[1], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                break;
	        case 3: if(kill(meComunicoConE[0], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                break;
	        case 4: if(kill(meComunicoConE[0], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(kill(meComunicoConE[1], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                break;
	    }
	}
	
	//Por aquí solo entra el proceso F. Hace una cosa u otra según la función tiempo() y devuelve lo necesario a C.
	if(!strcmp(argv[0], "F")){  
	    decision = tiempo();
	    switch(decision){
	        case 1: return 1;
	        case 2: if(kill(meComunicoConF[1], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(waitpid(meComunicoConF[1], &valordevuelto, 0) == -1){ perror("Error waitpid"); return -1;}
	                return 0;
	        case 3: if(kill(meComunicoConF[0], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                return 1;
	        case 4: if(kill(meComunicoConF[0], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(kill(meComunicoConF[1], SIGTERM) == -1){ perror("Error kill"); return -1;}
	                if(waitpid(meComunicoConF[1], &valordevuelto2, 0) == -1){ perror("Error waitpid"); return -1;}
	                return 0;
	    } 
	}	
	return 0;
}
