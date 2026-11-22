#ifndef MODULE_SETXPBAR_H
#define MODULE_SETXPBAR_H

#include "Player.h"
#include "ScriptMgr.h"
#include "WardenWin.h"
#include "Config.h"

extern bool sxp_Enabled;

const std::string _addonPayloadSetXpBar = R"lua(
local function SetXpRate(rate)
    SendChatMessage(".xp set "..rate, "SAY")
end
local xpRates = {
    {text="Experience:",isTitle=true},
    {text="x1 (Blizzlike)",func=function() SetXpRate(1) end},
    {text="x2",func=function() SetXpRate(2) end},
    {text="x3",func=function() SetXpRate(3) end},
    {text="x5",func=function() SetXpRate(5) end},
    {text="x7",func=function() SetXpRate(7) end},
    {text="x12",func=function() SetXpRate(12) end},
    {text="Custom",func=function() StaticPopup_Show("SET_XP_RATE") end}
}
local function InitializeMenu(self,level)
    if not level then return end
    for _,rate in pairs(xpRates) do
        local info=UIDropDownMenu_CreateInfo()
        info.text=rate.text
        info.func=rate.func
        info.isTitle=rate.isTitle
        UIDropDownMenu_AddButton(info,level)
    end
end
local xpBarMenu=CreateFrame("Frame","SetXpBarMenu",UIParent,"UIDropDownMenuTemplate")
xpBarMenu.initialize=InitializeMenu


-- elvui blizz xpbar support

local function OpenXpMenu(self,button)
    if button=="RightButton" then
        EasyMenu(xpRates,xpBarMenu,"cursor",3,-3,"MENU")
    end
end
if ElvUI_ExperienceBar then
    ElvUI_ExperienceBar:EnableMouse(true)
    ElvUI_ExperienceBar:HookScript("OnClick",OpenXpMenu)
elseif MainMenuExpBar then
    MainMenuExpBar:EnableMouse(true)
    MainMenuExpBar:HookScript("OnMouseDown",OpenXpMenu)
end

----------



StaticPopupDialogs["SET_XP_RATE"]={
    text="Enter custom XP rate:",
    button1="Set Rate",
    button2="Cancel",
    hasEditBox=true,
    OnAccept=function(self)
        local rate=self.editBox:GetText()
        SetXpRate(rate)
    end,
    timeout=0,
    whileDead=true,
    hideOnEscape=true,
    preferredIndex=3
}
)lua";

const std::string _prePayload = "wlbuf = '';";
const std::string _postPayload = "local a,b=loadstring(wlbuf)if not a then message(b)else a()end";
const std::string _midPayload = _addonPayloadSetXpBar;

const uint16 _prePayloadId = 9600;
const uint16 _postPayloadId = 9601;
const uint16 _tmpPayloadId = 9602;

class SetXpBarServerScript : public ServerScript
{
public:
    SetXpBarServerScript() : ServerScript("SetXpBarServerScript", { SERVERHOOK_CAN_PACKET_SEND }) {}

private:
    bool CanPacketSend(WorldSession* session, const WorldPacket& packet) override;
    std::vector<std::string> GetChunks(std::string s, uint8_t chunkSize);
    void SendChunkedPayload(Warden* warden, WardenPayloadMgr* payloadMgr, std::string payload, uint32 chunkSize);
};

class SetXpBarWorldScript : public WorldScript
{
public:
    SetXpBarWorldScript() : WorldScript("SetXpBarWorldScript", { WORLDHOOK_ON_AFTER_CONFIG_LOAD }) {}

private:
    void OnAfterConfigLoad(bool reload) override;
};

#endif // MODULE_SETXPBAR_H
