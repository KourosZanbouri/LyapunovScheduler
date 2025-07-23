
# Lyapunov-based QoS Scheduler for Simu5G

![Language](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Framework](https://img.shields.io/badge/OMNeT%2B%2B-6.1-green.svg)
![Library](https://img.shields.io/badge/Simu5G-1.3.0-orange.svg)
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



## 💻 4. Code Implementation Explained

The implementation of this theory is primarily contained within the `prepareSchedule()` function of `LyapunovScheduler.cc`. The scheduler's lifecycle in each time slot follows a clear pattern.

### Part 1: Gathering Inputs for All Active Connections

The scheduler's main `for` loop iterates through every active connection (`cid`) provided by the framework in the `carrierActiveConnectionSet_`. For each `cid`, it gathers the three key components of our metric.

**1. `Queue_Backlog` (The Urgency)**
This code block retrieves the current data queue size for the user. A critical part of this implementation is the workaround for a bug in the Simu5G framework where the BSR buffer map is incorrectly keyed by `MacCid` instead of `MacNodeId`. Our code correctly searches this map using the `cid` to find the associated queue size.

```cpp
// The BSR virtualBuffers map is incorrectly keyed by MacCid instead of MacNodeId.
// This workaround uses the current 'cid' to look up the queue size from the broken map.
double queueBacklog = 0;
for (auto const& [key_cid, buffer_ptr] : *virtualBuffers) {
    if (key_cid == cid) {
        if (buffer_ptr != nullptr) {
            queueBacklog = buffer_ptr->getQueueOccupancy();
        }
        break;
    }
}

```

**2. `Instantaneous_Rate` (The Opportunity)** This block queries the AMC (Adaptive Modulation and Coding) module to determine the potential data rate based on the UE's reported channel quality. It calculates the total bytes that could be transmitted across all available resource blocks.

```
unsigned int availableBlocks = 0, availableBytes = 0;
for (auto antenna : info.readAntennaSet()) {
    for (auto band : info.readBands()) {
        unsigned int blocks = eNbScheduler_->readAvailableRbs(nodeId, antenna, band);
        availableBlocks += blocks;
        availableBytes += eNbScheduler_->mac_->getAmc()->computeBytesOnNRbs(nodeId, band, blocks, dir, carrierFrequency_);
    }
}
// The Instantaneous Rate is effectively (availableBytes / availableBlocks)

```

**3. `QoS_Weight` (The Importance)** This block queries the `QfiContextManager` singleton to get the 5G QoS parameters associated with the current `cid` and computes a static priority weight based on a predefined formula.

```
const QfiContext* ctx = getQfiContextForCid(cid);
double qosWeight = ctx ? computeQosWeightFromContext(*ctx) : 1.0;

```

### Part 2: Calculating the Metric and Making the Decision

These three components are then combined in a direct implementation of our Weighted Max-Weight formula. The result (`s`) is logged for analysis and then pushed to a `std::priority_queue`. The priority queue is a highly efficient data structure that automatically sorts all the connections, ensuring that when we start granting resources, we begin with the connection that has the highest score.

```
// The direct implementation of the Lyapunov-based scheduling metric
double s = (availableBlocks > 0 && queueBacklog > 0)
               ? queueBacklog * (static_cast<double>(availableBytes) / availableBlocks) * qosWeight
               : 0.0;

// The priority queue makes the final scheduling decision
score.push({cid, s});

```

Finally, the `while (!score.empty())` loop iterates through the sorted connections and calls `requestGrant()` to allocate resources until all connections have been served or all radio resources for the time slot have been exhausted.

## 📊 5. Behavior Under Different Network Conditions

The true value of this scheduler is revealed when the network is under load.

-   **In an Uncongested Network:** When the total offered load is less than the channel capacity, any well-behaved scheduler will perform similarly. Since there are enough resources for all applications, both the Lyapunov scheduler and a PF scheduler will grant nearly 100% of the requested traffic. The results will appear almost identical.
    
-   **In a Congested Network:** This is where the Lyapunov scheduler excels. When the demand for resources exceeds the supply, it becomes ruthless and efficient.
    
    -   It will heavily prioritize the flows with the highest `QoS_Weight` to keep their queues small, ensuring their low-latency and high-reliability requirements are met.
        
    -   It will sacrifice the performance of low-priority, best-effort traffic, allowing its queues to grow and packets to be dropped if necessary to protect the critical flows.
        
    -   This behavior is in stark contrast to a PF scheduler, which would attempt to maintain fairness by "stealing" resources from high-priority flows to serve the low-priority ones, thereby violating the strict QoS hierarchy.
        

## 🚀 6. Conclusion and Future Work

This project successfully implements a QoS-aware scheduler based on the principles of Lyapunov optimization. Through simulation in a congested industrial 5G scenario, it has been shown to effectively protect high-priority, delay-sensitive traffic by prioritizing system stability over traditional fairness metrics.

A promising area for future research would be to implement a hybrid scheduler that combines the stability of the Lyapunov framework with the starvation-prevention of Proportional Fair. This could be achieved with a metric like:

$$\text{Score} = \frac{\log(1 + Q_{cid}) \times R_{cid} \times W_{cid}}{T_{avg}} $$
This would create a scheduler that is stable and QoS-aware in the short term, while still guaranteeing a baseline level of service to all users in the long term. 