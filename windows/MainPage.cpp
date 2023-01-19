#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include <iomanip>
#include <sstream>
#include <string>

using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Text;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Data::Json;

namespace winrt::ConnectMyPiToWiFi::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();

        // Determine if Bluetooth is enabled
        if (!SimpleBLE::Adapter::bluetooth_enabled()) {
            BTInterfaceStatusTextBox().Text(L"Bluetooth is DISABLED on the central device.");
        }
        else {
            BTInterfaceStatusTextBox().Text(L"Bluetooth is ENABLED on the central device.");
        }

        // Find available Bluetooth adapters
        adapters = SimpleBLE::Safe::Adapter::get_adapters();
        if (!adapters.has_value() || adapters->empty()) {
            OutputDebugStringA("No Bluetooth adapters found\n");
        }
        else {
            // Add identifier of each adapter to combo box
            OutputDebugStringA("Bluetooth adapters found\n");
            for (SimpleBLE::Safe::Adapter adapter : *adapters) {
                OutputDebugStringA("Adapter:");
                OutputDebugStringA(adapter.identifier().value().c_str());
                OutputDebugStringA("\n");
                OutputDebugStringA("Address:");
                OutputDebugStringA(adapter.address().value().c_str());
                OutputDebugStringA("\n");

                BTAdapterComboBox().Items().Append(box_value(to_hstring(adapter.identifier().value())));
            }
        }
    }
    

    // Check Bluetooth peripheral names for being reasonable (no-blank, etc.)
    boolean reasonableBluetoothName(std::string name) {
        // Discard blank BT devices
        if (!name.empty())
            return true;
        else
            return false;
    }


    void MainPage::ScanDevicesHandler(IInspectable const&, RoutedEventArgs const&)
    {
        // Show busy state (while waiting for GATT services)
        Core::CoreWindow window{ Core::CoreWindow::GetForCurrentThread() };
        window.PointerCursor(Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Wait, 0));

        // Get index of selected item (Select... is 0th index)
        int adapterIdx = BTAdapterComboBox().SelectedIndex();
        if (adapterIdx > 0) {
            BTAdapterRequired().Visibility(Visibility::Collapsed); // clear error

            // Access selected BT adapter (dec idx by 1 to account for 'Select...' item)
            adapterIdx--;
            SimpleBLE::Safe::Adapter& adapter = adapters->at(adapterIdx);

            // Scan for peripherals
            OutputDebugStringA("Scanning for devices...\n");
            adapter.scan_for(PERIPHERAL_SCAN_TIME);
            OutputDebugStringA("...done\n");

            // Get the list of peripherals found from scan (might be empty)
            std::optional<std::vector<SimpleBLE::Safe::Peripheral>> scanResults;
            scanResults = adapter.scan_get_results();
            if (!scanResults.has_value() || scanResults->empty()) {
                BTDeviceComboBox().Visibility(Visibility::Collapsed);
                OutputDebugStringA("No peripherals found\n");
            }
            else {
                // Add identifier of each peripheral to combo box
                BTDeviceComboBox().Items().Clear();
                BTDeviceComboBox().Items().Append(box_value(L"Select..."));
                peripherals->clear();
                for (SimpleBLE::Safe::Peripheral peripheral : *scanResults) {
                    if (reasonableBluetoothName(peripheral.identifier().value())) {
                        OutputDebugStringA("Peripheral:");
                        OutputDebugStringA(peripheral.identifier().value().c_str());
                        OutputDebugStringA("\n");
                        OutputDebugStringA("Address:");
                        OutputDebugStringA(peripheral.address().value().c_str());
                        OutputDebugStringA("\n");

                        // Add only reasonably named peripherals to final list
                        BTDeviceComboBox().Items().Append(box_value(to_hstring(peripheral.identifier().value())));
                        peripherals->push_back(peripheral);
                    }
                }
                BTDeviceComboBox().SelectedIndex(0); // set to 'Select...'
            }
        }
        else {
            BTAdapterRequired().Visibility(Visibility::Visible); // show error
        }
        // Enable the UI
        window.PointerCursor(Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Arrow, 0));
    }


    void MainPage::ApplyWifiHandler(IInspectable const&, RoutedEventArgs const&)
    {
        // Show busy state (while waiting for GATT services)
        Core::CoreWindow window{ Core::CoreWindow::GetForCurrentThread() };
        window.PointerCursor(Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Wait, 0));

        // Track required field validation status
        bool validated = true;

        // UI WiFi input vars
        std::string wifiName = "";
        std::string wifiCountry = "";
        std::string wifiSecurity = "";
        std::string peripheralDevice = "";
        std::string wifiPwd = "";

        // UI error text message
        std::string errorText;

        // Hide (reset) success and error text boxes
        BTErrorTextBox().Visibility(Visibility::Collapsed);
        BTSuccessTextBox().Visibility(Visibility::Collapsed);

        // Get index of selected item (Select... is 0th index)
        int deviceIdx = BTDeviceComboBox().SelectedIndex();
        if (deviceIdx > 0) {
            BTDeviceRequired().Visibility(Visibility::Collapsed); // clear error
            peripheralDevice = to_string(unbox_value<hstring>(BTDeviceComboBox().SelectedValue()));
            OutputDebugStringA("Peripheral Bluetooth device:");
            OutputDebugStringA(peripheralDevice.c_str());
            OutputDebugStringA("\n");
        }
        else {
            BTDeviceRequired().Visibility(Visibility::Visible); // show error
            validated = false;
        }

        // Get index of selected item (Select... is 0th index)
        int wifiCountryIdx = BTWifiCountryComboBox().SelectedIndex();
        if (wifiCountryIdx > 0) {
            BTWifiCountryRequired().Visibility(Visibility::Collapsed); // clear error
            wifiCountry = to_string(unbox_value<hstring>(BTWifiCountryComboBox().SelectedValue()));
            OutputDebugStringA("WiFi country:");
            OutputDebugStringA(wifiCountry.c_str());
            OutputDebugStringA("\n");
        }
        else {
            BTWifiCountryRequired().Visibility(Visibility::Visible); // show error
            validated = false;
        }

        wifiName = to_string(BTWifiNameTextBox().Text());
        if (!wifiName.empty()) {
            BTWifiNameRequired().Visibility(Visibility::Collapsed); // clear error
            OutputDebugStringA("WiFi name:");
            OutputDebugStringA(wifiName.c_str());
            OutputDebugStringA("\n");
        }
        else {
            BTWifiNameRequired().Visibility(Visibility::Visible); // show error
            validated = false;
        }

        // Get index of selected item (Select... is 0th index)
        int wifiSecurityIdx = BTWifiSecurityModeComboBox().SelectedIndex();
        if (wifiSecurityIdx > 0) {
            BTWifiSecurityRequired().Visibility(Visibility::Collapsed); // clear error
            wifiSecurity = to_string(unbox_value<hstring>(BTWifiSecurityModeComboBox().SelectedValue()));
            OutputDebugStringA("WiFi security mode:");
            OutputDebugStringA(wifiSecurity.c_str());
            OutputDebugStringA("\n");
        }
        else {
            BTWifiSecurityRequired().Visibility(Visibility::Visible); // show error
            validated = false;
        }

        wifiPwd = to_string(BTWifiPasswordBox().Password());
        if (!wifiPwd.empty()) {
            BTWifiPwdRequired().Visibility(Visibility::Collapsed); // clear error
            OutputDebugStringA("WiFi password:");
            OutputDebugStringA(wifiPwd.c_str());
            OutputDebugStringA("\n");
        }
        else {
            BTWifiPwdRequired().Visibility(Visibility::Visible); // show error
            validated = false;
        }

        // See if we passed data input validation
        if (validated) {
            // Access selected BT device (dec idx by 1 to account for 'Select...' item)
            SimpleBLE::Safe::Peripheral peripheral = peripherals->at(--deviceIdx);

            OutputDebugStringA("Connecting to peripheral...\n");
            bool success = peripheral.connect();
            OutputDebugStringA("...done\n");

            // Double check to see that the peripheral is actually connected
            if (success && peripheral.is_connected()) {
                // BT peripheral service and characteristic UUID's of interest
                SimpleBLE::BluetoothUUID const WLANMANAGE_SVC_UUID = "12634d89-d598-4874-8e86-7d042ee07ba7";
                SimpleBLE::BluetoothUUID const WLANMANAGE_CHR_CONFIGURE = "4116f8d2-9f66-4f58-a53d-fc7440e7c14e";
                SimpleBLE::BluetoothUUID const WLANMANAGE_CHR_RESTART = "9c7dbce8-de5f-4168-89dd-74f04f4e5842";
                SimpleBLE::BluetoothUUID const WLANMANAGE_CHR_GETMACADDR = "16637984-be04-49b8-be43-86cf4efda929";

                // Retrieve peripheral services
                services = peripheral.services();
                if (!services.has_value() || services->empty()) {
                    OutputDebugStringA("Failed to retrieve services\n");
                    errorText = "Failed to retrieve services";
                }
                else {
                    // Verify that our service(s) are supported by connected peripheral
                    bool serviceSupported = false;
                    for (SimpleBLE::Service service : *services) {
                        for (auto characteristic : service.characteristics()) {
                            if (service.uuid() == WLANMANAGE_SVC_UUID && characteristic.uuid() == WLANMANAGE_CHR_CONFIGURE) {
                                serviceSupported = true;
                                break;
                            }
                        }
                    }

                    if (serviceSupported) {
                        // Create JSON keys for (Linux) WPA_SUPPLICANT
                        static wchar_t wifiCountryKey[] = L"country";
                        static wchar_t wifiSsidKey[] = L"ssid";
                        static wchar_t wifiScanSsidKey[] = L"scan_ssid";
                        static wchar_t wifiPskKey[] = L"psk";
                        static wchar_t wifiKeyMgmtKey[] = L"key_mgmt";

                        // BT peripheral requires (WPA_SUPPLICANT) params to be JSON encoded
                        JsonObject wpaSupplicant;
                        wpaSupplicant.SetNamedValue(wifiCountryKey, JsonValue::CreateStringValue(to_hstring(wifiCountry)));
                        wpaSupplicant.SetNamedValue(wifiSsidKey, JsonValue::CreateStringValue(to_hstring(wifiName)));
                        wpaSupplicant.SetNamedValue(wifiScanSsidKey, JsonValue::CreateStringValue(to_hstring(L"1")));
                        wpaSupplicant.SetNamedValue(wifiPskKey, JsonValue::CreateStringValue(to_hstring(wifiPwd)));
                        wpaSupplicant.SetNamedValue(wifiKeyMgmtKey, JsonValue::CreateStringValue(to_hstring(wifiSecurity)));
                        std::string wpaSupplicantStr = to_string(wpaSupplicant.Stringify()); // already UTF-8 encoded

                        // Write new network parameters to peripheral device
                        // Just to make certain that SimplBLE calls don't throw exceptions
                        try {
                            success = peripheral.write_request(WLANMANAGE_SVC_UUID, WLANMANAGE_CHR_CONFIGURE, wpaSupplicantStr);
                            if (success) {
                                OutputDebugStringA("WiFi network parameters successfully written\n");
                            }
                            else {
                                OutputDebugStringA("Failed to write WiFi network parameters\n");
                                errorText = "Failed to write WiFi network parameters";
                            }
                        }
                        catch (...) {
                            OutputDebugStringA("An error occured trying to write WiFi network parameters\n");
                            errorText = "An error occured trying to write WiFi network parameters";
                        }

                        // Restart peripheral device network interface
                        // Just to make certain that SimplBLE calls don't throw exceptions
                        try {
                            std::string restartStr = "RESTART"; // already UTF-8 encoded
                            success = peripheral.write_request(WLANMANAGE_SVC_UUID, WLANMANAGE_CHR_RESTART, restartStr);
                            if (success) {
                                OutputDebugStringA("Peripheral device WiFi interface restarted\n");
                            }
                            else {
                                OutputDebugStringA("Failed to restart WiFi interface on peripheral device\n");
                                errorText = "Failed to restart WiFi interface on peripheral device";
                            }
                        }
                        catch (...)
                        {
                            OutputDebugStringA("An error occured trying to restart WiFi interface on peripheral device\n");
                            errorText = "An error occured trying to restart WiFi interface on peripheral device";
                        }

                        // Read the MAC address of the peripheral wireless adapter
                        // Just to make certain that SimplBLE calls don't throw exceptions
                        try {
                            std::optional<SimpleBLE::ByteArray> rx_data = peripheral.read(WLANMANAGE_SVC_UUID, WLANMANAGE_CHR_GETMACADDR);
                            if (!rx_data.has_value() || rx_data->empty()) {
                                OutputDebugStringA("Failed to read MAC address\n");
                                errorText = "Failed to read MAC address";
                            }
                            else {
                                // Got MAC address, so display it
                                OutputDebugStringA("WiFi MAC address is ");
                                OutputDebugStringA(rx_data.value().c_str());
                                OutputDebugStringA("\n");
                                BTErrorTextBox().Visibility(Visibility::Collapsed);
                                BTSuccessTextBox().Visibility(Visibility::Visible);
                                BTSuccessTextBox().Text(L"WiFi settings updated: peripheral MAC address is " + to_hstring(rx_data.value()));
                            }
                        }
                        catch (...)
                        {
                            OutputDebugStringA("An error occured trying to read MAC address\n");
                            errorText = "An error occured trying to read MAC address";
                        }
                    }
                    else {
                        OutputDebugStringA("Selected peripheral does not support the required services\n");
                        errorText = "Selected peripheral does not support the required services";
                    }
                }

                // All done
                // The SimpleBLE peripheral.disconnect() mehtod does not work properly, so the source was modified
                // to remove the call to Close(). Otherwise, the disconnect() would fail, and would have been
                // commented out here.
                peripheral.disconnect();
            }
            else {
                OutputDebugStringA("Failed to connect to peripheral device\n");
                errorText = "Failed to connect to peripheral device";
            }
            if (!errorText.empty()) {
                // Have an error, so display it
                BTSuccessTextBox().Visibility(Visibility::Collapsed);
                BTErrorTextBox().Visibility(Visibility::Visible);
                BTErrorTextBox().Text(to_hstring(errorText));
            }
        }

        // Enable the UI
        window.PointerCursor(Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Arrow, 0));
    }
}


void ConnectMyPiToWiFi::implementation::MainPage::BTAdapterComboBox_SelectionChanged(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const&)
{
    int adapterIdx = BTAdapterComboBox().SelectedIndex();
    if (adapterIdx > 0) {
        BTAdapterRequired().Visibility(Visibility::Collapsed); // clear error
    }
}


void ConnectMyPiToWiFi::implementation::MainPage::BTDeviceComboBox_SelectionChanged(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const&)
{
    int deviceIdx = BTDeviceComboBox().SelectedIndex();
    if (deviceIdx > 0) {
        BTDeviceRequired().Visibility(Visibility::Collapsed); // clear error
    }
}


void ConnectMyPiToWiFi::implementation::MainPage::BTWifiCountryComboBox_SelectionChanged(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const&) {
    int deviceIdx = BTWifiCountryComboBox().SelectedIndex();
    if (deviceIdx > 0) {
        BTWifiCountryRequired().Visibility(Visibility::Collapsed); // clear error
    }
}

void ConnectMyPiToWiFi::implementation::MainPage::BTWifiSecurityModeComboBox_SelectionChanged(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const&)
{
    int deviceIdx = BTWifiSecurityModeComboBox().SelectedIndex();
    if (deviceIdx > 0) {
        BTWifiSecurityRequired().Visibility(Visibility::Collapsed); // clear error
    }
}


void ConnectMyPiToWiFi::implementation::MainPage::BTWifiNameTextBox_CharacterReceived(Windows::UI::Xaml::UIElement const&, Windows::UI::Xaml::Input::CharacterReceivedRoutedEventArgs const&)
{
    BTWifiNameRequired().Visibility(Visibility::Collapsed); // clear error
}


void ConnectMyPiToWiFi::implementation::MainPage::BTWifiPasswordBox_CharacterReceived(Windows::UI::Xaml::UIElement const&, Windows::UI::Xaml::Input::CharacterReceivedRoutedEventArgs const&)
{
    BTWifiPwdRequired().Visibility(Visibility::Collapsed); // clear error
}
