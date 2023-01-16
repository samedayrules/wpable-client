#pragma once
#include "MainPage.g.h"
#include <simpleble/SimpleBLE.h>

constexpr auto PERIPHERAL_SCAN_TIME = 15000; // msec

namespace winrt::ConnectMyPiToWiFi::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

    public:
        void ScanDevicesHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
        void ApplyWifiHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
        void BTAdapterComboBox_SelectionChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void BTDeviceComboBox_SelectionChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void BTWifiSecurityModeComboBox_SelectionChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void BTWifiNameTextBox_CharacterReceived(Windows::UI::Xaml::UIElement const& sender, Windows::UI::Xaml::Input::CharacterReceivedRoutedEventArgs const& args);
        void BTWifiPasswordBox_CharacterReceived(Windows::UI::Xaml::UIElement const& sender, Windows::UI::Xaml::Input::CharacterReceivedRoutedEventArgs const& args);
    private:
        std::optional<std::vector<SimpleBLE::Safe::Adapter>> adapters = std::vector<SimpleBLE::Safe::Adapter>();
        std::optional<std::vector<SimpleBLE::Safe::Peripheral>> peripherals = std::vector<SimpleBLE::Safe::Peripheral>();
        std::optional<std::vector<SimpleBLE::Service>> services = std::vector<SimpleBLE::Service>();
    };

}

namespace winrt::ConnectMyPiToWiFi::factory_implementation
{ 
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
