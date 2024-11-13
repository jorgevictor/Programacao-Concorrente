// Programa que implementa um diálogo entre a função principal e uma goroutine usando canais
package main

import (
	"fmt"
	"time"
)

func tarefa(chanMainToGoroutine chan string, chanGoroutineToMain chan string) {
	// Recebe a primeira mensagem da função principal
	msgFromMain := <-chanMainToGoroutine
	fmt.Println("Goroutine recebeu:", msgFromMain)

	// Envia resposta para a função principal
	msgToMain := "Oi Main, bom dia, tudo bem?"
	chanGoroutineToMain <- msgToMain

	// Recebe a segunda mensagem da função principal
	msgFromMain = <-chanMainToGoroutine
	fmt.Println("Goroutine recebeu:", msgFromMain)

	// Envia segunda resposta para a função principal
	msgToMain = "Certo, entendido."
	chanGoroutineToMain <- msgToMain

	// Goroutine imprime: finalizando
	fmt.Println("Goroutine imprime: finalizando")
}

func main() {
	// Cria canais para comunicação
	chanMainToGoroutine := make(chan string)
	chanGoroutineToMain := make(chan string)

	// Inicia a goroutine
	go tarefa(chanMainToGoroutine, chanGoroutineToMain)

	// Main envia primeira mensagem para a goroutine
	msgToGoroutine := "Olá, Goroutine, bom dia!"
	chanMainToGoroutine <- msgToGoroutine

	// Main recebe resposta da goroutine
	msgFromGoroutine := <-chanGoroutineToMain
	fmt.Println("Main recebeu:", msgFromGoroutine)

	// Main envia segunda mensagem para a goroutine
	msgToGoroutine = "Tudo bem! Vou terminar tá?"
	chanMainToGoroutine <- msgToGoroutine

	// Main recebe segunda resposta da goroutine
	msgFromGoroutine = <-chanGoroutineToMain
	fmt.Println("Main recebeu:", msgFromGoroutine)

	time.Sleep(time.Second) //bloqueio na main
	
	// Main imprime: finalizando
	fmt.Println("Main imprime: finalizando")
	
}
