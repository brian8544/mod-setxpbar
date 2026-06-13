#include "SetXpBar.h"
#include "Config.h"
#include "Log.h"

bool sxp_Enabled = false;

std::vector<std::string> SetXpBarServerScript::GetChunks(std::string s, uint8_t chunkSize)
{
    std::vector<std::string> chunks;
    for (uint32_t i = 0; i < s.size(); i += chunkSize)
        chunks.push_back(s.substr(i, chunkSize));
    return chunks;
}

void SetXpBarServerScript::SendChunkedPayload(Warden* warden, WardenPayloadMgr* payloadMgr, std::string payload, uint32 chunkSize)
{
    auto chunks = GetChunks(payload, chunkSize);

    if (!payloadMgr->GetPayloadById(_prePayloadId))
        payloadMgr->RegisterPayload(_prePayload, _prePayloadId);

    payloadMgr->QueuePayload(_prePayloadId);
    warden->ForceChecks();

    for (auto const& chunk : chunks)
    {
        std::string escapedChunk = chunk;
        size_t pos = 0;
        while ((pos = escapedChunk.find("]]", pos)) != std::string::npos)
            escapedChunk.replace(pos, 2, "] ]");

        auto smallPayload = "wlbuf = wlbuf .. [[" + escapedChunk + "]];";

        payloadMgr->RegisterPayload(smallPayload, _tmpPayloadId, true);
        payloadMgr->QueuePayload(_tmpPayloadId);
        warden->ForceChecks();
    }

    if (!payloadMgr->GetPayloadById(_postPayloadId))
        payloadMgr->RegisterPayload(_postPayload, _postPayloadId);

    payloadMgr->QueuePayload(_postPayloadId);
    warden->ForceChecks();
}

bool SetXpBarServerScript::CanPacketSend(WorldSession* session, const WorldPacket& packet)
{
    if (!sxp_Enabled)
        return true;

    if (packet.GetOpcode() != SMSG_LOGIN_VERIFY_WORLD)
        return true;

    WardenWin* warden = (WardenWin*)session->GetWarden();
    if (!warden || !warden->IsInitialized())
        return true;

    auto payloadMgr = warden->GetPayloadMgr();
    if (!payloadMgr)
        return true;

    payloadMgr->ClearQueuedPayloads();
    payloadMgr->UnregisterPayload(_prePayloadId);
    payloadMgr->UnregisterPayload(_postPayloadId);
    payloadMgr->UnregisterPayload(_tmpPayloadId);

    SendChunkedPayload(warden, payloadMgr, _midPayload, 128);

    return true;
}

void SetXpBarWorldScript::OnAfterConfigLoad(bool /*reload*/)
{
    sxp_Enabled = sConfigMgr->GetOption<bool>("SetXpBar.Enable", false);
}

void AddSetXpBarScripts()
{
    new SetXpBarServerScript();
    new SetXpBarWorldScript();
}
