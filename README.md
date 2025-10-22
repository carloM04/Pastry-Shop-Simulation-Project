# 🍰 Pastry Shop Simulation — Politecnico di Milano (August 2024)

## 📘 Overview
This project implements a **discrete-time simulation of an industrial pastry shop**, developed as part of the *“Algoritmi e Strutture Dati”* final exam at **Politecnico di Milano (A.Y. 2023–2024)**.

The goal of the assignment was **not only to produce a correct simulation**, but also to design it with **high time and memory efficiency**, using the most appropriate **data structures and algorithms** for each part of the problem.

---

## 🚀 Objectives

- ✅ Implement a correct and complete simulation of the pastry shop system.  
- ⚙️ Achieve **optimal algorithmic performance** by minimizing time complexity.  
- 🧠 Use **efficient data structures** (e.g., priority queues, balanced trees, hash maps, queues).  
- 💾 Keep **memory usage low** while maintaining clarity and modularity of the code.  

The project was evaluated primarily on **correctness**, **asymptotic efficiency**, and **code design choices**.

---

## 🧩 Simulation Description

The program simulates the behavior of a pastry factory operating in discrete time.  
After reading each command from standard input, one unit of time passes.

The main entities in the simulation are:

- **Ingredients:** Each identified by name and tracked in the warehouse with multiple lots, each having a quantity and expiration time.  
- **Recipes:** Each recipe lists ingredient requirements and quantities.  
- **Orders:** Clients can order pastries, which are prepared only when ingredients are available.  
- **Warehouse:** Ingredients are consumed FIFO by expiration date, and expired lots are discarded.  
- **Courier (Truck):** Periodically picks up prepared orders, subject to capacity limits and loading rules.

---

## 🧮 Input Format

The input file begins with:
