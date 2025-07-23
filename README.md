# Lyapunov-based QoS Scheduler for Simu5G

![Language](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Framework](https://img.shields.io/badge/OMNeT%2B%2B-6.0-green.svg)
![Library](https://img.shields.io/badge/Simu5G-1.2.2-orange.svg)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)

A custom uplink/downlink scheduler for the Simu5G/OMNeT++ framework that uses Lyapunov optimization to provide enhanced Quality of Service (QoS) differentiation and queue stability for 5G networks.

---

## 📖 Project Motivation

In 5G networks, especially in scenarios with diverse requirements like industrial automation, it's not enough to be "fair." Schedulers must intelligently prioritize traffic to meet strict QoS targets, such as low delay for critical control signals and high throughput for video streams. This project implements a scheduler based on **Lyapunov optimization**, a powerful mathematical tool from control theory, to manage network resources with a primary focus on **system stability**. Unlike fairness-based schedulers (like Proportional Fair), this scheduler's main goal is to keep data queues from growing uncontrollably, thereby minimizing delay and preventing packet drops for high-priority traffic.

---

## 🧠 Core Concept: Lyapunov Optimization

Lyapunov optimization provides a framework for making online decisions in a stochastic system to keep it stable.

### The Lyapunov Function: A Measure of Instability

At its core is the **Lyapunov function**, which is a scalar value that represents the "energy" or "pressure" in a system. For a system to be stable, its energy should not increase over time. In a communication network, the system state is the set of all data queue backlogs, $Q(t)$. The Lyapunov function is defined as the sum of the squares of these backlogs:

$$ L(Q(t)) = \frac{1}{2}\sum_{i} Q_{i}(t)^2 $$

A large value of $L(Q(t))$ means the network is highly congested and unstable.

### The Lyapunov Drift: Controlling the Change

The **Lyapunov drift**, $\Delta L(t)$, is the expected change in the function over one time slot. The goal of our scheduler is to make a decision in each time slot that pushes this drift to be as negative as possible, thereby "draining" energy from the system and making it more stable.

$$ \Delta L(t) = \mathbb{E}[L(Q(t+1)) - L(Q(t)) \mid Q(t)] $$

By choosing the action that minimizes this drift, we are inherently prioritizing the users that will lead to the largest reduction in overall queue pressure. This naturally leads to a scheduling rule known as a **Max-Weight** algorithm.

---

## 📈 The Scheduling Metric: A Weighted Max-Weight Algorithm

The rule that minimizes the Lyapunov drift is one that maximizes the product of queue size and service rate. To incorporate different service priorities, we use a **Weighted Max-Weight** approach.

The core metric used by this scheduler is:
$$ \text{Score} = \text{Queue\_Backlog} \times \text{Instantaneous\_Rate} \times \text{QoS\_Weight} $$
This combines the three factors essential for intelligent scheduling:
* **Urgency (`Queue_Backlog`)**: How much data is waiting? This is the primary term derived from the Lyapunov drift.
* **Opportunity (`Instantaneous_Rate`)**: How good is the user's channel right now?
* **Importance (`QoS_Weight`)**: How critical is this specific data flow?

> #### An Important Refinement
> In practice, a raw `Queue_Backlog` can grow so large that it overpowers the `QoS_Weight`. A more robust implementation (and the recommended version for research) uses the logarithm of the queue to dampen this effect, making the scheduler more responsive to the QoS priority:
> $$ \text{Score} = \log(1 + \text{Queue\_Backlog}) \times \text{Instantaneous\_Rate} \times \text{QoS\_Weight} $$

---

## 💻 Code Implementation Explained

The implementation of this theory can be seen directly in the `prepareSchedule()` function. The code first gathers the necessary inputs and then calculates the metric for each active connection.

### Part 1: Gathering Inputs

The scheduler's main `for` loop iterates through each active connection (`cid`) and gathers the three key components of our metric.

**1. `Queue_Backlog` (The Urgency)**
This code block retrieves the current data queue size for the user. This is the implementation of the $Q$ term in our formula.
```cpp
// ============================= START OF WORKAROUND =============================
// The BSR virtualBuffers map is incorrectly keyed by MacCid instead of MacNodeId.
// This workaround uses the current 'cid' to look up the queue size from the broken map.
double queueBacklog = 0;
for (auto const& [key_cid, buffer_ptr] : *virtualBuffers) {
    // The key in the map is a MacCid, so we compare it to our current cid
    if (key_cid == cid) {
        if (buffer_ptr != nullptr) {
            queueBacklog = buffer_ptr->getQueueOccupancy();
        }
        break; // Found our entry, no need to search further
    }
}
// ============================== END OF WORKAROUND ==============================