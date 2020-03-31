#include "corpus_harvester_plugin.h"

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>
#include <flutter/standard_method_codec.h>
#include <sys/utsname.h>
#include <memory>
#include <sstream>

namespace {

    class CorpusHarvesterPlugin : public flutter::Plugin {
        public:
            static void RegisterWithRegistrar(flutter::PluginRegistrar *registrar);

            // Creates a plugin that communicates on the given channel.
            CorpusHarvesterPlugin(std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel);

            virtual ~CorpusHarvesterPlugin();
        
        private:
            // Called when a method is called on |channel_|
            void HandleMethodCall(
                const flutter::MethodCall<flutter::EncodableValue> &method_call,
                std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

            // The MethodChannel used for communication with the Flutter engine.
            std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel_;
    };

    // static
    void CorpusHarvesterPlugin::RegisterWithRegistrar(flutter::PluginRegistrar *registrar) {
        auto channel = std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "corpus_harvester_plugin",
            &flutter::StandardMethodCodec::GetInstance());
        auto *channel_pointer = channel.get();

        auto plugin = std::make_unique<CorpusHarvesterPlugin>(std::move(channel));

        channel_pointer->SetMethodCallHandler(
            [plugin_pointer = plugin.get()](const auto &call, auto result) {
                plugin_pointer->HandleMethodCall(call, std::move(result));
            });

        registrar->AddPlugin(std::move(plugin));
    }

    CorpusHarvesterPlugin::CorpusHarvesterPlugin(
        std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel)
        : channel_(std::move(channel)) {}

    CorpusHarvesterPlugin::~CorpusHarvesterPlugin() {}

    void CorpusHarvesterPlugin::HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue> &method_call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {

        if(method_call.method_name().compare("getPlatformVersion") == 0) {
            struct utsname uname_data = {};
            uname(&uname_data);
            std::ostringstream version_stream;
            version_stream << "Linux" << uname_data.version;
            flutter::EncodableValue response(version_stream.str());
            result->Success(&response);
        } else {
            result->NotImplemented();
        }
    }
} // namespace

void CorpusHarvesterPluginRegisterWithRegistrar(FlutterDesktopPluginRegistrarRef registrar) {
    // The plugin registrar owns the plugin, registered callbacks, etc, so must remain valid for the life of the application.
    static auto *plugin_registrar = new flutter::PluginRegistrar(registrar);

    CorpusHarvesterPlugin::RegisterWithRegistrar(plugin_registrar);
}