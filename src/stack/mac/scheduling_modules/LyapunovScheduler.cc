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



LyapunovScheduler::LyapunovScheduler(Binder* binder)
    : LteScheduler(binder)
{
    loadContextIfNeeded();
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
    double weight = 1.0;
    weight *= (10.0 - ctx.fiveQi + 1);  // Lower 5QI = better
    if (ctx.isGbr) weight *= 2.0;
    weight *= 10.0 / (ctx.priorityLevel + 1);  // Lower priority = better
    if (ctx.delayBudgetMs <= 10) weight *= 5.0;
    else if (ctx.delayBudgetMs <= 50) weight *= 3.0;
    else if (ctx.delayBudgetMs <= 100) weight *= 1.5;
    EV << NOW << "Qfi: "<< ctx.qfi << " 5QI: "<< ctx.fiveQi << " Weight: "<< weight << endl;
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



void LyapunovScheduler::prepareSchedule()
{
    EV << NOW << " LyapunovScheduler::prepareSchedule (Lyapunov Optimization)" << endl;

    const LteMacBufferMap* virtualBuffers = eNbScheduler_->mac_->getBsrVirtualBuffers();
    if (!virtualBuffers) {
        EV_WARN << "LyapunovScheduler: Could not retrieve BSR virtual buffers map." << endl;
        return;
    }

    grantedBytes_.clear();
    activeConnectionTempSet_ = *activeConnectionSet_;

    auto compare = [](const ScoredCid& a, const ScoredCid& b) { return a.second < b.second; };
    std::priority_queue<ScoredCid, std::vector<ScoredCid>, decltype(compare)> score(compare);

    for (const auto& cid : carrierActiveConnectionSet_) {
        MacNodeId nodeId = MacCidToNodeId(cid);
        OmnetId id = binder_->getOmnetId(nodeId);
        grantedBytes_[cid] = 0;

        if (nodeId == NODEID_NONE || id == 0) {
            activeConnectionSet_->erase(cid);
            activeConnectionTempSet_.erase(cid);
            carrierActiveConnectionSet_.erase(cid);
            continue;
        }

        Direction dir = (direction_ == UL)
                        ? ((MacCidToLcid(cid) == D2D_SHORT_BSR) ? D2D
                          : (MacCidToLcid(cid) == D2D_MULTI_SHORT_BSR) ? D2D_MULTI
                          : UL)
                        : DL;

        if (dir != UL && dir != DL) continue;

        const UserTxParams& info = eNbScheduler_->mac_->getAmc()->computeTxParams(nodeId, dir, carrierFrequency_);
        if (info.readCqiVector().empty() || info.readBands().empty()) continue;
        if (eNbScheduler_->allocatedCws(nodeId) == info.getLayers().size()) continue;

        unsigned int availableBlocks = 0, availableBytes = 0;
        for (auto antenna : info.readAntennaSet()) {
            for (auto band : info.readBands()) {
                unsigned int blocks = eNbScheduler_->readAvailableRbs(nodeId, antenna, band);
                availableBlocks += blocks;
                availableBytes += eNbScheduler_->mac_->getAmc()->computeBytesOnNRbs(nodeId, band, blocks, dir, carrierFrequency_);
            }
        }

        const QfiContext* ctx = getQfiContextForCid(cid);
        double qosWeight = ctx ? computeQosWeightFromContext(*ctx) : 1.0;


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


        double s = (availableBlocks > 0 && queueBacklog > 0)
                       ? queueBacklog * (static_cast<double>(availableBytes) / availableBlocks) * qosWeight
                       : 0.0;

        double instRate = availableBlocks > 0 ? (static_cast<double>(availableBytes) / availableBlocks) : 0.0;
        EV_INFO << "METRIC_CALC -> CID: " << cid << " | NodeId: " << nodeId << " | QoS_W: " << qosWeight
                << " | Queue: " << queueBacklog << " | InstRate: " << instRate << " | Score: " << s << endl;

        s += uniform(getEnvir()->getRNG(0), -scoreEpsilon_, scoreEpsilon_);

        score.push({cid, s});
    }

    while (!score.empty()) {
        ScoredCid current = score.top();
        MacCid cid = current.first;
        bool terminate = false, active = true, eligible = true;

        unsigned int granted = requestGrant(cid, UINT32_MAX, terminate, active, eligible);
        grantedBytes_[cid] += granted;

        if (terminate) break;
        if (!active || !eligible) score.pop();
        if (!active) {
            activeConnectionTempSet_.erase(cid);
            carrierActiveConnectionSet_.erase(cid);
        }
    }
}



void LyapunovScheduler::commitSchedule()
{
    // This function was used by the Proportional Fair scheduler to update the
    // historical average throughput. For the Lyapunov-based scheduler, which is
    // stateless (it only considers the current queue state), this is no longer needed.
    *activeConnectionSet_ = activeConnectionTempSet_;
}

} // namespace simu5g
