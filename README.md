# i2c-sensor-driver

##  License Declaration

```c
// SPDX-License-Identifier: MIT
```
- Declares the license using the SPDX identifier.
- Required for kernel contributions; this one uses **GPL v2**.

---

##  Header Inclusions

```c
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/slab.h>
```
- `module.h`: For kernel module infrastructure.
- `i2c.h`: For I²C device and driver APIs.
- `hwmon.h` and `hwmon-sysfs.h`: For integrating with the hwmon subsystem.
- `slab.h`: For memory allocation (`kzalloc`).

---

## Register Definition

```c
#define ACME_REG_TEMP 0x00
```
- Defines the register address where the sensor stores temperature data.

---

## Driver Data Structure

```c
struct acme_data {
    struct i2c_client *client;
};
```
- Holds per-device data.
- `i2c_client` represents the I²C device instance.

---

##  Temperature Read Function

```c
static int acme_read_temp(struct acme_data *data, long *val)
```
- Reads a 16-bit word from the temperature register.
- Converts it from **big-endian** to host-endian.
- Stores the result in `val` (assumed to be in milli-Celsius).
- Returns 0 on success or a negative error code.

---

##  hwmon Read Callback

```c
static int acme_hwmon_read(...)
```
- Called by the hwmon subsystem when userspace reads `temp1_input`.
- Verifies the sensor type and attribute.
- Delegates to `acme_read_temp()`.

---

##  hwmon Operations

```c
static const struct hwmon_ops acme_hwmon_ops = {
    .read = acme_hwmon_read,
};
```
- Defines the read callback for hwmon.

---

## hwmon Channel Info

```c
static const struct hwmon_channel_info *acme_info[] = {
    HWMON_CHANNEL_INFO(temp, HWMON_T_INPUT),
    NULL
};
```
- Declares that this driver exposes one temperature input.

---

## hwmon Chip Info

```c
static const struct hwmon_chip_info acme_chip_info = {
    .ops = &acme_hwmon_ops,
    .info = acme_info,
};
```
- Combines the ops and channel info into a single structure.

---

## Probe Function

```c
static int acme_probe(struct i2c_client *client, ...)
```
- Called when the kernel matches this driver to a device.
- Allocates memory for `acme_data`.
- Stores the `i2c_client` pointer.
- Registers the device with hwmon using `devm_hwmon_device_register_with_info()`.
- Returns 0 on success or an error code.

---

## Device Matching Tables

```c
static const struct i2c_device_id acme_id[] = { ... };
static const struct of_device_id acme_of_match[] = { ... };
```
- Allow matching via legacy board files (`i2c_device_id`) or Device Tree (`of_device_id`).

---

## Driver Registration

```c
static struct i2c_driver acme_driver = { ... };
module_i2c_driver(acme_driver);
```
- Defines the driver and registers it with the kernel.
- `module_i2c_driver()` sets up init/exit functions automatically.

---

## Module Metadata

```c
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("ACME Temperature Sensor Driver (hwmon)");
MODULE_LICENSE("GPL");
```
- Provides metadata for tools like `modinfo`.

---

##  What This Driver Does

- Talks to a simple I²C temperature sensor.
- Reads temperature from a register.
- Exposes it via the standard hwmon interface (`/sys/class/hwmon/.../temp1_input`).
- Compatible with tools like `sensors`.
