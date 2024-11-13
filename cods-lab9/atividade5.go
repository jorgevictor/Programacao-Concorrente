package main

import (
	"fmt"
	"math"
	"sync"
)

// Função para verificar se um número é primo
func ehPrimo(n int) bool {
	if n <= 1 {
		return false
	}
	if n == 2 {
		return true
	}
	if n%2 == 0 {
		return false
	}
	limite := int(math.Sqrt(float64(n))) + 1
	for i := 3; i < limite; i += 2 {
		if n%i == 0 {
			return false
		}
	}
	return true
}

func main() {
	N := 100000 // Limite superior da sequência de números (1 a N)
	M := 10     // Número de goroutines (workers)

	numeros := make(chan int, 100) // Canal para enviar os números a serem testados
	resultados := make(chan bool)  // Canal para receber os resultados dos testes

	var wg sync.WaitGroup

	// Inicia M goroutines para verificar primalidade
	for i := 0; i < M; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for num := range numeros {
				primo := ehPrimo(num)
				resultados <- primo
			}
		}()
	}

	// Goroutine para gerar os números de 1 a N
	go func() {
		for i := 1; i <= N; i++ {
			numeros <- i
		}
		close(numeros) // Fecha o canal após enviar todos os números
	}()

	// Goroutine para fechar o canal de resultados após todas as goroutines terminarem
	go func() {
		wg.Wait()
		close(resultados)
	}()

	// Coleta os resultados e conta quantos números primos foram encontrados
	totalPrimos := 0
	for primo := range resultados {
		if primo {
			totalPrimos++
		}
	}

	fmt.Printf("Total de números primos entre 1 e %d: %d\n", N, totalPrimos)
}
