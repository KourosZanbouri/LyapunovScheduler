# 5G Hybrid Lyapunov Scheduler for Simu5G

![Language](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Framework](https://img.shields.io/badge/OMNeT%2B%2B-6.x-green.svg)
![Library](https://img.shields.io/badge/Simu5G-1.3.0-orange.svg)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)

A custom, tunable uplink/downlink scheduler for the Simu5G/OMNeT++ framework that uses Lyapunov optimization to provide enhanced Quality of Service (QoS) differentiation and queue stability for 5G networks.

---

## 📖 Project Motivation

In 5G networks, especially in scenarios with diverse requirements like industrial automation, it's not enough to be "fair." Schedulers must intelligently prioritize traffic to meet strict QoS targets, such as low delay for critical control signals and high throughput for video streams. This project implements a scheduler based on **Lyapunov optimization**, a powerful mathematical tool from control theory, to manage network resources with a primary focus on **system stability**. Unlike fairness-based schedulers (like Proportional Fair), this scheduler's main goal is to keep data queues from growing uncontrollably, thereby minimizing delay and preventing packet drops for high-priority traffic.

---

## 🧠 Core Concept: Lyapunov Optimization

Lyapunov optimization provides a framework for making online decisions in a stochastic system to keep it stable.

### The Lyapunov Function: A Measure of Instability

At its core is the **Lyapunov function**, a scalar value that represents the "energy" or "pressure" in a system. For a system to be stable, its energy should not increase over time. In a communication network, the system state is the set of all data queue backlogs, $Q(t)$. The Lyapunov function is defined as the sum of the squares of these backlogs:

$$ L(Q(t)) = \frac{1}{2}\sum_{i} Q_{i}(t)^2 $$

A large value of $L(Q(t))$ means the network is highly congested and unstable.

### The Lyapunov Drift: Controlling the Change

The **Lyapunov drift**, $\Delta L(t)$, is the expected change in the function over one time slot. The goal of our scheduler is to make a decision in each time slot that pushes this drift to be as negative as possible, thereby "draining" energy from the system and making it more stable.

$$ \Delta L(t) = \mathbb{E}[L(Q(t+1)) - L(Q(t)) \mid Q(t)] $$

By choosing the action that minimizes this drift, we are inherently prioritizing the users that will lead to the largest reduction in overall queue pressure.

---

## 📈 The Scheduling Metric: A Tunable Weighted Max-Weight Algorithm

The rule that minimizes the Lyapunov drift can be extended to a **Weighted Max-Weight** algorithm that incorporates QoS. This scheduler uses a tunable version of this metric to provide precise control over its behavior.

The final score for each user is calculated as:
$$ Score = Q(t)^\alpha \cdot R(t) \cdot W^\beta $$

This combines three factors essential for intelligent scheduling:
* **Urgency ($Q(t)^\alpha$)**: How much data is waiting in the queue, amplified by the `alpha` exponent. A higher `alpha` makes the scheduler more sensitive to growing queues, aggressively working to control latency.
* **Opportunity ($R(t)$)**: How good is the user's channel quality right now? This is the instantaneous achievable data rate.
* **Importance ($W^\beta$)**: How critical is this data flow? This is a static weight derived from 5G QoS parameters (QFI, priority level, etc.) and amplified by the `beta` exponent. A higher `beta` makes the scheduler more ruthless in enforcing the QoS hierarchy.

---

## ✨ Features

* **Unified Hybrid Logic:** Implements a single-pass scheduling logic that provides strict priority for mission-critical traffic (via score bonus) and utility-based scheduling for all other flows.
* **Aggressive QoS Differentiation:** Employs an exponential weighting scheme to create massive mathematical separation between traffic classes.
* **Tunable Performance:** Fully configurable via `.ini` file parameters (`alpha` and `beta`) to adjust the scheduler's sensitivity to queue growth and its QoS aggressiveness.
* **Stable & Robust:** The final implementation is simplified to a single priority queue, making it efficient and free of the logical bugs present in multi-list approaches.

---

## 🔧 System Prerequisites

* **OMNeT++:** Version 6.0 or newer
* **INET Framework:** Version 4.5 or newer
* **Simu5G:** Version 1.3.0

---

## 🛠️ Installation & Setup

Follow these steps to integrate the custom Lyapunov scheduler into your Simu5G project.

### 1. Add Scheduler Source Files

Place the following two files into your Simu5G project directory at **`src/stack/mac/scheduling_modules/`**.

* `LyapunovScheduler.h`
* `LyapunovScheduler.cc`

*(The full code for these files is provided below.)*

### 2. Register the Scheduler in the Framework

You must modify two Simu5G framework files to make the simulator aware of your new scheduler.

#### a) Modify `LteMacEnb.ned`

Open **`src/stack/mac/LteMacEnb.ned`**. You need to add `LYAPUNOV_SCHEDULER` to the `schedulerType` enum and declare the `alpha` and `beta` parameters.

```ned
// In src/stack/mac/LteMacEnb.ned
simple LteMacEnb
{
    parameters:
        // ... (other parameters)

        // --> ADD "LYAPUNOV_SCHEDULER" to this list
        string schedulingDisciplineDl @enum(DRR,PF,MAXCI,MAXCI_MB,MAXCI_OPT_MB,MAXCI_COMP,ALLOCATOR_BESTFIT,QOS_PF,LYAPUNOV_SCHEDULER) = default("MAXCI");
        string schedulingDisciplineUl @enum(DRR,PF,MAXCI,MAXCI_MB,MAXCI_OPT_MB,MAXCI_COMP,ALLOCATOR_BESTFIT,QOS_PF,LYAPUNOV_SCHEDULER) = default("MAXCI");
        

        // --> ADD these two lines for your scheduler's parameters
        double alpha = default(1.0);
        double beta = default(1.0);

    // ... (rest of the file)
}
```

#### b) Modify `LteSchedulerEnb.cc`

Open **`src/stack/mac/scheduler/LteSchedulerEnb.cc`**. Add a `case` to the `getScheduler` function so the framework knows how to create an instance of your scheduler.

```cpp
// In src/stack/mac/scheduler/LteSchedulerEnb.cc, inside the getScheduler() function

// FIND THIS:
// case QOS_PF:
//     return new QoSAwareScheduler(binder_, mac_->par("pfAlpha").doubleValue());

// --> ADD YOUR CASE AFTER IT:
        case LYAPUNOV_SCHEDULER:
            return new LyapunovScheduler(binder_, mac_->par("alpha").doubleValue(), mac_->par("beta").doubleValue());
```

### 3. Recompile Simu5G

After saving all file changes, clean and rebuild your entire Simu5G project.

```bash
make clean
make
```

---

## ⚙️ Configuration

To use the scheduler, configure your `.ini` and QoS mapping files.

### `omnetpp.ini` Configuration

```ini
# 1. Select your custom scheduler
**.gNBs.lteNic.mac.schedulerType = "LYAPUNOV_SCHEDULER"

# 2. Provide the tuning parameters for your scheduler
**.gNBs.lteNic.mac.scheduler.alpha = 1.5
**.gNBs.lteNic.mac.scheduler.beta = 1.2

# 3. Ensure queues are large enough to prevent artificial packet drops
*.gnb.**.queueSize = 204800B
*.ue[*].**.queueSize = 204800B

---

## 💻 Code Implementation

These are the complete, final source files for the custom scheduler.

### `LyapunovScheduler.h`

```cpp
/*
 * LyapunovScheduler.h
 *
 * Author: kouros
 */

#ifndef __SIMU5G_LYAPUNOVSCHEDULER_H_
#define __SIMU5G_LYAPUNOVSCHEDULER_H_

#include <omnetpp.h>
#include "stack/mac/scheduler/LteScheduler.h"
#include "stack/sdap/common/QfiContextManager.h"

namespace simu5g {

class LteSchedulerEnb;

class LyapunovScheduler : public LteScheduler
{
  protected:
    // Manager for QoS Flow Identifier contexts
    QfiContextManager* qfiContextMgr_ = nullptr;
    bool contextLoaded_ = false;

    double alpha_;
    double beta_;

    // Map to store granted bytes in the current TTI for each connection
    std::map<MacCid, unsigned int> grantedBytes_;

    // Temporary set of active connections for the current scheduling period
    ActiveSet activeConnectionTempSet_;

    // Small epsilon value for floating point comparisons and randomization
    const double scoreEpsilon_ = 1e-6;

    typedef std::pair<MacCid, double> ScoredCid;

    // --- Methods ---

    // Initializes the QFI context manager
    void loadContextIfNeeded();

    // Retrieves the QFI context for a given Connection ID (CID)
    const QfiContext* getQfiContextForCid(MacCid cid);

    // Calculates a weight based on the QoS parameters of a flow
    double computeQosWeightFromContext(const QfiContext& ctx);


  public:
    // Constructor - Simplified to remove PF parameters
    LyapunovScheduler(Binder* binder, double alpha, double beta);


    // Main scheduling functions
    void prepareSchedule() override;
    void commitSchedule() override;
};

} // namespace simu5g

#endif // __SIMU5G_LYAPUNOVSCHEDULER_H_

```

### `LyapunovScheduler.cc`

```cpp

/*
 * LyapunovScheduler.cc
 *
 * Author: kouros
 */

#include "stack/mac/scheduling_modules/LyapunovScheduler.h"
#include "stack/mac/scheduler/LteSchedulerEnb.h"
#include "stack/mac/LteMacEnb.h"
#include "stack/mac/buffer/LteMacBuffer.h"

namespace simu5g {

using namespace omnetpp;



// Constructor saves alpha and beta using an initializer list
LyapunovScheduler::LyapunovScheduler(Binder* binder, double alpha, double beta)
    : LteScheduler(binder), alpha_(alpha), beta_(beta)
{
    loadContextIfNeeded();
    EV << "LyapunovScheduler created with alpha: " << alpha_ << ", beta: " << beta_ << endl;
}


void LyapunovScheduler::loadContextIfNeeded()
{
    if (!contextLoaded_) {
        qfiContextMgr_ = QfiContextManager::getInstance();  // singleton
        ASSERT(qfiContextMgr_ != nullptr);
        contextLoaded_ = true;
    }
}

double LyapunovScheduler::computeQosWeightFromContext(const QfiContext& ctx)
{
    // Base weight for all flows
    double weight = 1.0;

    // --- Exponential Priority Scaling ---
    // Use a base value greater than 1. A higher base creates more separation.
    const double priorityBase = 2.0;
    // Lower priorityLevel is better (e.g., 1 is higher priority than 9).
    // This creates an exponential gap: level 1 gets 2^8, level 9 gets 2^0.
    weight *= pow(priorityBase, 9 - ctx.priorityLevel);

    // --- Delay Budget Scaling ---
    // Extremely aggressive bonus for tight delay budgets (URLLC-like)
    if (ctx.delayBudgetMs <= 10) {
        weight *= 10.0;
    } else if (ctx.delayBudgetMs <= 50) {
        weight *= 3.0;
    }

    // --- GBR Bonus ---
    // Provide a significant, constant multiplier for guaranteed bit rate flows.
    if (ctx.isGbr) {
        weight *= 2.0;
    }

    EV_INFO << NOW << " LyapunovScheduler [QFI=" << ctx.qfi << ", PrioLvl=" << ctx.priorityLevel << "]"
            << " --> Computed Aggressive Weight: " << weight << endl;

    return weight;
}

const QfiContext* LyapunovScheduler::getQfiContextForCid(MacCid cid)
{
    if (!qfiContextMgr_) return nullptr;
    int qfi = qfiContextMgr_->getQfiForCid(cid);
    if (qfi < 0) {
        EV_WARN << "LyapunovScheduler: No QFI registered for CID " << cid << "\n";
        return nullptr;
    }
    return qfiContextMgr_ -> getContextByQfi(qfi);
}



struct SchedulingInfo {
    MacCid cid;
    const QfiContext* qfiContext;
    double queueBacklog;
    double achievableRate; // in bytes per resource block
};

void LyapunovScheduler::prepareSchedule()
{
    EV << NOW << " HybridLyapunovScheduler::prepareSchedule (Final Fixed Version)" << endl;

    const LteMacBufferMap* virtualBuffers = (direction_ == UL) ? eNbScheduler_->mac_->getBsrVirtualBuffers() : nullptr;
    grantedBytes_.clear();
    activeConnectionTempSet_ = *activeConnectionSet_;

    // --- Unified priority queue for all traffic ---
    auto compare = [](const ScoredCid& a, const ScoredCid& b) { return a.second < b.second; };
    std::priority_queue<ScoredCid, std::vector<ScoredCid>, decltype(compare)> scoreQueue(compare);

    // --- Single Pass Data Gathering and Scoring ---
    for (const auto& cid : carrierActiveConnectionSet_)
    {
        MacNodeId nodeId = MacCidToNodeId(cid);
        if (nodeId == NODEID_NONE || binder_->getOmnetId(nodeId) == 0) continue;

        double backlog = 0;
        Direction dir = (direction_ == UL) ? UL : DL;

        if (dir == DL) {
            backlog = eNbScheduler_->mac_->getDlQueueSize(cid);
        } else { // Uplink
            if (virtualBuffers && virtualBuffers->count(cid) > 0) {
                backlog = virtualBuffers->at(cid)->getQueueOccupancy();
            }
        }
        if (backlog == 0) continue;

        const UserTxParams& info = eNbScheduler_->mac_->getAmc()->computeTxParams(nodeId, dir, carrierFrequency_);
        if (info.readCqiVector().empty() || info.readBands().empty()) continue;

        unsigned int availableBlocks = 0, availableBytes = 0;
        for (auto antenna : info.readAntennaSet()) {
            for (auto band : info.readBands()) {
                unsigned int blocks = eNbScheduler_->readAvailableRbs(nodeId, antenna, band);
                availableBlocks += blocks;
                availableBytes += eNbScheduler_->mac_->getAmc()->computeBytesOnNRbs(nodeId, band, blocks, dir, carrierFrequency_);
            }
        }
        double achievableRate = (availableBlocks > 0) ? static_cast<double>(availableBytes) / availableBlocks : 0.0;
        if (achievableRate == 0) continue;

        const QfiContext* ctx = getQfiContextForCid(cid);
        double qosWeight = ctx ? computeQosWeightFromContext(*ctx) : 1.0;

        // --- Score calculation with tuning exponents ---
        double score = pow(backlog, alpha_) * achievableRate * pow(qosWeight, beta_);

        // --- Correct Strict Priority logic using a massive score bonus ---
        if (ctx && ctx->qfi == 4) { // QFI 4 for URLLC
            score *= 1e12;
        }

        score += uniform(getEnvir()->getRNG(0), -scoreEpsilon_, scoreEpsilon_);

        EV_INFO << NOW << " LyapunovScheduler [CID=" << cid << ", QFI=" << (ctx ? ctx->qfi : -1) << "]"
                << " Backlog(Q^a)=" << pow(backlog, alpha_)
                << " Rate(R)=" << achievableRate
                << " Weight(W^b)=" << pow(qosWeight, beta_)
                << " --> FINAL SCORE=" << score << endl;

        scoreQueue.push({cid, score});
    }

    // --- Unified Granting Loop ---
    while (!scoreQueue.empty())
    {
        ScoredCid current = scoreQueue.top();
        scoreQueue.pop();

        bool terminate = false, active = true, eligible = true;
        unsigned int granted = requestGrant(current.first, UINT32_MAX, terminate, active, eligible);
        grantedBytes_[current.first] += granted;

        if (terminate) break;
        if (!active) {
            activeConnectionTempSet_.erase(current.first);
            carrierActiveConnectionSet_.erase(current.first);
        }
    }
}






void LyapunovScheduler::commitSchedule()
{
    *activeConnectionSet_ = activeConnectionTempSet_;
}

} // namespace simu5g
```

---

## 📊 Expected Outcome

When running a simulation with heavy congestion, the `LyapunovScheduler` should demonstrate significantly better QoS protection for high-priority flows compared to a baseline `QOS_PF` scheduler. This will be visible in the final statistics as:
* **Higher Throughput & Lower Latency** for high-priority flows (e.g., QFI 4, 5).
* **Significantly Lower Throughput** for the best-effort flow (e.g., QFI 7), which is intentionally starved to protect the critical traffic.

This behavior proves the effectiveness of the stability-oriented approach in maintaining strict QoS guarantees.

---

## 📜 License

This project is licensed under the MIT License.