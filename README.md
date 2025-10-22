# Pasticceria Simulation — Politecnico di Milano (August 2024)

## Overview

This project implements a **discrete-time simulation of an industrial pastry shop**, developed as part of the *“Algoritmi e Strutture Dati”* final exam at **Politecnico di Milano (A.Y. 2023–2024)**.

The goal of the assignment was not only to produce a correct simulation, but also to design it with **high time and memory efficiency**, using the most appropriate **data structures and algorithms** for each part of the problem.

---

## Objectives

- Implement a correct and complete simulation of the pastry shop system.  
- Achieve **optimal algorithmic performance** by minimizing time complexity.  
- Use **efficient data structures** such as priority queues, balanced trees, hash maps, and queues.  
- Maintain **low memory usage** while keeping the code modular and clear.  

The project was evaluated primarily on correctness, asymptotic efficiency, and design choices.

---

## Simulation Description

The program simulates the behavior of a pastry factory operating in discrete time.  
After reading each command from standard input, one unit of time passes.

The main entities in the simulation are:

- **Ingredients:** Each identified by name and tracked in the warehouse with multiple lots, each having a quantity and expiration time.  
- **Recipes:** Each recipe lists ingredient requirements and quantities.  
- **Orders:** Clients can order pastries, which are prepared only when ingredients are available.  
- **Warehouse:** Ingredients are consumed FIFO by expiration date, and expired lots are discarded.  
- **Courier (Truck):** Periodically picks up prepared orders, subject to capacity limits and loading rules.

---

## Input Format

The input file begins with:
<delivery_period><truck_capacity>

Then follows a sequence of commands:

| Command | Description | Example |
|----------|--------------|----------|
| `aggiungi_ricetta <nome_ricetta> <ingrediente> <quantità> ...` | Adds a recipe to the catalog | `aggiungi_ricetta torta farina 50 uova 10 zucchero 20` |
| `rimuovi_ricetta <nome_ricetta>` | Removes a recipe (if no pending orders) | `rimuovi_ricetta torta` |
| `rifornimento <ingrediente> <quantità> <scadenza> ...` | Adds ingredient lots to inventory | `rifornimento zucchero 200 150 farina 1000 220` |
| `ordine <nome_ricetta> <numero_elementi>` | Places a new order | `ordine ciambella 6` |

Each command prints a corresponding output message (e.g., `aggiunta`, `rifornito`, `accettato`, etc.).

---

## Delivery Logic

At every multiple of the courier’s period, the program prints the list of orders loaded on the truck:
<arrival_time><recipe_name><quatity)

Orders are selected in chronological order until the truck’s weight limit is reached, then loaded in decreasing order of weight (and by arrival time in case of ties).  
If the truck is empty, the message `camioncino vuoto` is printed.

---

## Algorithmic Design

To ensure speed and scalability, the implementation was based on efficient data management principles:

- **Hash tables** for constant-time access to recipes and ingredients.  
- **Priority queues (min-heaps)** for selecting the nearest expiration date of ingredient lots.  
- **FIFO queues** for pending orders.  
- **Balanced structures** or sorting routines for courier loading operations.  

This guarantees near-optimal performance in:
- `O(log n)` for insertions and extractions from queues.  
- `O(1)` for lookups in maps or dictionaries.  
- Linear or sublinear complexity for all core operations.

---

## Testing

The repository contains multiple test cases inside the tests folder.

Each pair defines an input scenario and the expected program output.

To verify correctness:
```bash
./final_program < tests/test_1.txt > my_output.txt
diff my_output.txt tests/test_1_output.txt
```
If there are no differences, the implementation passes the test.
