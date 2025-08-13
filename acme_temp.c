
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/slab.h>

#define ACME_REG_TEMP 0x00  // Register that holds temperature

struct acme_data {
    struct i2c_client *client;
};

static int acme_read_temp(struct acme_data *data, long *val)
{
    int ret;
    u16 raw;
    struct i2c_client *client = data->client;

    ret = i2c_smbus_read_word_data(client, ACME_REG_TEMP);
    if (ret < 0)
        return ret;

    raw = be16_to_cpu(ret); // Convert from big-endian if needed
    *val = raw; // Already in milli-Celsius
    return 0;
}

static int acme_hwmon_read(struct device *dev, enum hwmon_sensor_types type,
                           u32 attr, int channel, long *val)
{
    struct acme_data *data = dev_get_drvdata(dev);

    if (type != hwmon_temp || attr != hwmon_temp_input)
        return -EOPNOTSUPP;

    return acme_read_temp(data, val);
}

static const struct hwmon_ops acme_hwmon_ops = {
    .read = acme_hwmon_read,
};

static const struct hwmon_channel_info *acme_info[] = {
    HWMON_CHANNEL_INFO(temp, HWMON_T_INPUT),
    NULL
};

static const struct hwmon_chip_info acme_chip_info = {
    .ops = &acme_hwmon_ops,
    .info = acme_info,
};

static int acme_probe(struct i2c_client *client,
                      const struct i2c_device_id *id)
{
    struct acme_data *data;
    struct device *hwmon_dev;

    data = devm_kzalloc(&client->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->client = client;
    i2c_set_clientdata(client, data);

    hwmon_dev = devm_hwmon_device_register_with_info(&client->dev,
                                                     "acme_temp",
                                                     data,
                                                     &acme_chip_info,
                                                     NULL);
    return PTR_ERR_OR_ZERO(hwmon_dev);
}

static const struct i2c_device_id acme_id[] = {
    { "acme_temp", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, acme_id);

static const struct of_device_id acme_of_match[] = {
    { .compatible = "acme,temp-sensor" },
    { }
};
MODULE_DEVICE_TABLE(of, acme_of_match);

static struct i2c_driver acme_driver = {
    .driver = {
        .name = "acme_temp",
        .of_match_table = acme_of_match,
    },
    .probe = acme_probe,
    .id_table = acme_id,
};

module_i2c_driver(acme_driver);

MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("ACME Temperature Sensor Driver (hwmon)");
MODULE_LICENSE("MIT");

