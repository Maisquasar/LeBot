#include "DeleteMessageCommand.h"

#include "Bot.h"

void DeleteMessageCommand::Execute(const dpp::interaction_create_t& event)
{
    int64_t count = std::get<int64_t>(event.get_parameter("amount"));

    event.thinking();
    dpp::message_map messages;
    std::atomic_bool done = false;
    auto& handle = p_bot->GetHandle();
    auto channelId = event.command.channel_id;
    handle.messages_get(channelId, count, {}, {}, {},
        [&](const dpp::confirmation_callback_t& callback)
        {
            if (callback.is_error()) {
                std::cerr << "Failed to get messages: " << callback.get_error().message << std::endl;
                return;
            }

            messages = std::get<dpp::message_map>(callback.value);

            std::vector<dpp::snowflake> ids;
            std::vector<dpp::snowflake> oldIds;
            auto now_t = std::time(nullptr);  // current time as time_t
            for (const auto& [messageID, message] : messages)
            {
                double secs = std::difftime(now_t, message.sent);
                if (secs < 14 * 24 * 3600) {
                    // < 14 days → bulk delete
                    ids.push_back(messageID);
                }
                else {
                    // ≥ 14 days → single delete
                    oldIds.push_back(messageID);
                }
                p_bot->Log("Delete message ({}): {}", std::to_string(messageID), message.content);
                
            }
            handle.message_delete_bulk(ids, channelId, 
                [&](const dpp::confirmation_callback_t& del_cb) {
                    if ( del_cb.is_error() )
                        std::cerr << "Bulk delete failed: " << del_cb.get_error().message << "\n";
                    done = true;
            });

            if (!oldIds.empty()) {
                for (const auto& id : oldIds) {
                    handle.message_delete(channelId, id, 
                        [&](const dpp::confirmation_callback_t& del_cb) {
                            if ( del_cb.is_error() )
                                std::cerr << "Single delete failed: " << del_cb.get_error().message << "\n";
                            done = true;
                    });
                }
            }
        }
    );
    while (!done)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    event.delete_original_response();
}
