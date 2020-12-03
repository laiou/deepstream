/*
 * Copyright (c) 2018-2020, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <string.h>
#include "deepstream_app.h"
#include "deepstream_config_file_parser.h"

#define CONFIG_GROUP_APP "application"
#define CONFIG_GROUP_APP_ENABLE_PERF_MEASUREMENT "enable-perf-measurement"
#define CONFIG_GROUP_APP_PERF_MEASUREMENT_INTERVAL "perf-measurement-interval-sec"
#define CONFIG_GROUP_APP_GIE_OUTPUT_DIR "gie-kitti-output-dir"
#define CONFIG_GROUP_APP_GIE_TRACK_OUTPUT_DIR "kitti-track-output-dir"

#define CONFIG_GROUP_TESTS "tests"
#define CONFIG_GROUP_TESTS_FILE_LOOP "file-loop"

GST_DEBUG_CATEGORY_EXTERN (APP_CFG_PARSER_CAT);


#define CHECK_ERROR(error) \
    if (error) { \
        GST_CAT_ERROR (APP_CFG_PARSER_CAT, "%s", error->message); \
        goto done; \
    }
//NvDsSourceConfig 结构体，具体定义在apps-commom/includes/deepstream_sources.h，主要包括了depstream中source模块的相关参数。。这里的source是说的deepstream配置文件中的[source]
//不是gstreamer中的source组件，但是这些参数确是在使用gstreamer中的source组件的时候的配置。。
NvDsSourceConfig global_source_config;
//参数化source_list
static gboolean
parse_source_list (NvDsConfig * config, GKeyFile * key_file,
    gchar * cfg_file_path)
{
  gboolean ret = FALSE;
  gchar **keys = NULL;
  gchar **key = NULL;
  GError *error = NULL;
  gsize num_strings;
//通过g_key_file_get_keys返回组名为source-list的所有keys的数组，以NUll结尾
  keys = g_key_file_get_keys (key_file, CONFIG_GROUP_SOURCE_LIST, NULL, &error);
  CHECK_ERROR (error);

  for (key = keys; *key; key++) {
    //CONFIG_GROUP_SOURCE_LIST_NUM_SOURCE_BINS表示：num-source-bins具体定义在deepstream_config_file_parse.h
    //g_strcmp0用于比较两个字符串，依次比较两个字符串，返回一个小于，等于或者大于0的整数。。根据ASCLL逐字符比较。。遇到空字符或者末尾结束。。
    //小于0，表示前面的字符串小于后面的。。以此类推其他结果
    if (!g_strcmp0 (*key, CONFIG_GROUP_SOURCE_LIST_NUM_SOURCE_BINS)) {
      //g_key_file_get_integer将CONFIG_GROUP_SOURCE_LIST这个grope下CONFIG_GROUP_SOURCE_LIST_NUM_SOURCE_BINS键关联的值作为整数返回。。
      //这里的NUM_SOURCE_BINS应该是对应的命令行输入源的uri的模式
      config->total_num_sources =
          g_key_file_get_integer (key_file, CONFIG_GROUP_SOURCE_LIST,
          CONFIG_GROUP_SOURCE_LIST_NUM_SOURCE_BINS, &error);
      CHECK_ERROR (error);
    } else if (!g_strcmp0 (*key, CONFIG_GROUP_SOURCE_LIST_URI_LIST)) {
      //功能同上，返回一个list，其中num_strings表示list长度
      config->uri_list =
          g_key_file_get_string_list (key_file, CONFIG_GROUP_SOURCE_LIST,
          CONFIG_GROUP_SOURCE_LIST_URI_LIST, &num_strings, &error);
          //判断读取的source是否超出最大限制
      if (num_strings > MAX_SOURCE_BINS) {
        NVGSTDS_ERR_MSG_V ("App supports max %d sources", MAX_SOURCE_BINS);
        goto done;
      }
      CHECK_ERROR (error);
    } else {
      NVGSTDS_WARN_MSG_V ("Unknown key '%s' for group [%s]", *key,
          CONFIG_GROUP_SOURCE_LIST);
    }
  }
//查看GKeyFile文件中CONFIG_GROUP_SOURCE_LIST组下面是否存在CONFIG_GROUP_SOURCE_LIST_URI_LIST的密钥。。
  //补全上面的关于URI_LIST读取之后的total_num_sources的赋值
  if (g_key_file_has_key (key_file, CONFIG_GROUP_SOURCE_LIST,
          CONFIG_GROUP_SOURCE_LIST_URI_LIST, &error)) {
    if (g_key_file_has_key (key_file, CONFIG_GROUP_SOURCE_LIST,
            CONFIG_GROUP_SOURCE_LIST_NUM_SOURCE_BINS, &error)) {
      if (num_strings != config->total_num_sources) {
        NVGSTDS_ERR_MSG_V ("Mismatch in URIs provided and num-source-bins.");
        goto done;
      }
    } else {
      config->total_num_sources = num_strings;
    }
  }

  ret = TRUE;
done:
  if (error) {
    g_error_free (error);
  }
  if (keys) {
    g_strfreev (keys);
  }
  if (!ret) {
    NVGSTDS_ERR_MSG_V ("%s failed", __func__);
  }
  return ret;
}
//生成某个具体的源的配置
static gboolean
set_source_all_configs (NvDsConfig * config, gchar * cfg_file_path)
{
  guint i = 0;
  for (i = 0; i < config->total_num_sources; i++) {
    //将之前生成的参数化的具体source组的配置导入进来，可以看成是source组的通用配置。。实际上有点差别。。
    config->multi_source_config[i] = global_source_config;
    config->multi_source_config[i].camera_id = i;
    if (config->uri_list) {
      char *uri = config->uri_list[i];
      //接下来就是导入具体的uri了，以及不同类型的uri的独有参数的导入
      if (g_str_has_prefix (config->uri_list[i], "file://")) {
        config->multi_source_config[i].type = NV_DS_SOURCE_URI;
        config->multi_source_config[i].uri = g_strdup (uri + 7);
        config->multi_source_config[i].uri =
          g_strdup_printf ("file://%s",
              get_absolute_file_path (cfg_file_path,
                config->multi_source_config[i].uri));
      } else if (g_str_has_prefix (config->uri_list[i], "rtsp://")) {
        config->multi_source_config[i].type = NV_DS_SOURCE_RTSP;
        config->multi_source_config[i].uri = config->uri_list[i];
      } else {
        gchar *source_id_start_ptr = uri + 4;
        gchar *source_id_end_ptr = NULL;
        long camera_id =
            g_ascii_strtoull (source_id_start_ptr, &source_id_end_ptr, 10);
        if (source_id_start_ptr == source_id_end_ptr
            || *source_id_end_ptr != '\0') {
          NVGSTDS_ERR_MSG_V
              ("Incorrect URI for camera source %s. FORMAT: <usb/csi>:<dev_node/sensor_id>",
              uri);
          return FALSE;
        }
        if (g_str_has_prefix (config->uri_list[i], "csi:")) {
          config->multi_source_config[i].type = NV_DS_SOURCE_CAMERA_CSI;
          config->multi_source_config[i].camera_csi_sensor_id = camera_id;
        } else if (g_str_has_prefix (config->uri_list[i], "usb:")) {
          config->multi_source_config[i].type = NV_DS_SOURCE_CAMERA_V4L2;
          config->multi_source_config[i].camera_v4l2_dev_node = camera_id;
        } else {
          NVGSTDS_ERR_MSG_V ("URI %d (%s) not in proper format.", i,
              config->uri_list[i]);
          return FALSE;
        }
      }
    }
  }
  return TRUE;
}
//完成tests组的参数化配置
static gboolean
parse_tests (NvDsConfig *config, GKeyFile *key_file)
{
  gboolean ret = FALSE;
  gchar **keys = NULL;
  gchar **key = NULL;
  GError *error = NULL;

  keys = g_key_file_get_keys (key_file, CONFIG_GROUP_TESTS, NULL, &error);
  CHECK_ERROR (error);

  for (key = keys; *key; key++) {
    if (!g_strcmp0 (*key, CONFIG_GROUP_TESTS_FILE_LOOP)) {
      config->file_loop =
          g_key_file_get_integer (key_file, CONFIG_GROUP_TESTS,
          CONFIG_GROUP_TESTS_FILE_LOOP, &error);
      CHECK_ERROR (error);
    } else {
      NVGSTDS_WARN_MSG_V ("Unknown key '%s' for group [%s]", *key,
          CONFIG_GROUP_TESTS);
    }
  }

  ret = TRUE;
done:
  if (error) {
    g_error_free (error);
  }
  if (keys) {
    g_strfreev (keys);
  }
  if (!ret) {
    NVGSTDS_ERR_MSG_V ("%s failed", __func__);
  }
  return ret;
}

static gboolean
parse_app (NvDsConfig *config, GKeyFile *key_file, gchar *cfg_file_path)
{
  gboolean ret = FALSE;
  gchar **keys = NULL;
  gchar **key = NULL;
  GError *error = NULL;

  keys = g_key_file_get_keys (key_file, CONFIG_GROUP_APP, NULL, &error);
  CHECK_ERROR (error);

  for (key = keys; *key; key++) {
    if (!g_strcmp0 (*key, CONFIG_GROUP_APP_ENABLE_PERF_MEASUREMENT)) {
      config->enable_perf_measurement =
          g_key_file_get_integer (key_file, CONFIG_GROUP_APP,
          CONFIG_GROUP_APP_ENABLE_PERF_MEASUREMENT, &error);
      CHECK_ERROR (error);
    } else if (!g_strcmp0 (*key, CONFIG_GROUP_APP_PERF_MEASUREMENT_INTERVAL)) {
      config->perf_measurement_interval_sec =
          g_key_file_get_integer (key_file, CONFIG_GROUP_APP,
          CONFIG_GROUP_APP_PERF_MEASUREMENT_INTERVAL, &error);
      CHECK_ERROR (error);
    } else if (!g_strcmp0 (*key, CONFIG_GROUP_APP_GIE_OUTPUT_DIR)) {
      config->bbox_dir_path = get_absolute_file_path (cfg_file_path,
          g_key_file_get_string (key_file, CONFIG_GROUP_APP,
          CONFIG_GROUP_APP_GIE_OUTPUT_DIR, &error));
      CHECK_ERROR (error);
    } else if (!g_strcmp0 (*key, CONFIG_GROUP_APP_GIE_TRACK_OUTPUT_DIR)) {
      config->kitti_track_dir_path = get_absolute_file_path (cfg_file_path,
          g_key_file_get_string (key_file, CONFIG_GROUP_APP,
          CONFIG_GROUP_APP_GIE_TRACK_OUTPUT_DIR, &error));
      CHECK_ERROR (error);
    } else {
      NVGSTDS_WARN_MSG_V ("Unknown key '%s' for group [%s]", *key,
                          CONFIG_GROUP_APP);
    }
  }

  ret = TRUE;
done:
  if (error) {
    g_error_free (error);
  }
  if (keys) {
    g_strfreev (keys);
  }
  if (!ret) {
    NVGSTDS_ERR_MSG_V ("%s failed", __func__);
  }
  return ret;
}

//参数化配置文件，接收一个NvDsConfig结构体和一个配置文件路径
gboolean
parse_config_file (NvDsConfig *config, gchar *cfg_file_path)
{
  //Glib中通过GKeyFile来读取配置文件，即将cfg_file_path对应文件的内容读取出来，按照格式写入cfg_file中生成能被应用程序直接利用的配置文件
  GKeyFile *cfg_file = g_key_file_new ();
  GError *error = NULL;
  gboolean ret = FALSE;
  gchar **groups = NULL;
  gchar **group;
  guint i, j;
//config是一个NvDsConfig结构体
  config->source_list_enabled = FALSE;

  if (!APP_CFG_PARSER_CAT) {
    GST_DEBUG_CATEGORY_INIT (APP_CFG_PARSER_CAT, "NVDS_CFG_PARSER", 0, NULL);
  }
//g_key_file_load_from_file将密钥文件从内存加载到空的GKeyFile结构中
  if (!g_key_file_load_from_file (cfg_file, cfg_file_path, G_KEY_FILE_NONE,
          &error)) {
    GST_CAT_ERROR (APP_CFG_PARSER_CAT, "Failed to load uri file: %s",
        error->message);
    goto done;
  }
//在加载内容的GKeyFile中查找是否存在CONFIG_GROUP_SOURCE_LIST的分组，CONFIG_GROUP_SOURCE_LIST具体定义在depstream_config_file_parser.h中
//表示deepstream配置文件中的source-list
  //这里完成了config->total_num_sources的赋值
  //deepstream的配置里面是没有source-list组的。。这里判断它是否存在。。赋值时不存在也会自动添加这一组。。后面会删除。。具体可以参照GkeyFile的相关格式和操作
  if (g_key_file_has_group (cfg_file, CONFIG_GROUP_SOURCE_LIST)) {
    if (!parse_source_list (config, cfg_file, cfg_file_path)) {
      GST_CAT_ERROR (APP_CFG_PARSER_CAT, "Failed to parse '%s' group",
          CONFIG_GROUP_SOURCE_LIST);
      goto done;
    }
    config->num_source_sub_bins = config->total_num_sources;
    config->source_list_enabled = TRUE;
    if (!g_key_file_has_group (cfg_file, CONFIG_GROUP_SOURCE_ALL)) {
      NVGSTDS_ERR_MSG_V ("[source-attr-all] group not present.");
      ret = FALSE;
      goto done;
    }
    //g_key_file_remove_group 用它在当前的gkeyfile文件中删除source-list组，本身deepstream的配置文件不包括source-list组，上面自动添加了这一组。。现在删除
    g_key_file_remove_group (cfg_file, CONFIG_GROUP_SOURCE_LIST, &error);
  }
  //功能如上。。先判断是否存在source-attr-all组
  if (g_key_file_has_group (cfg_file, CONFIG_GROUP_SOURCE_ALL)) {
    //这里是生成对source组件的配置信息,parse_source参数化具体source组件的配置，实现参照deepstream_config_file_parser.c
    if (!parse_source (&global_source_config,
            cfg_file, CONFIG_GROUP_SOURCE_ALL, cfg_file_path)) {
      GST_CAT_ERROR (APP_CFG_PARSER_CAT, "Failed to parse '%s' group",
          CONFIG_GROUP_SOURCE_LIST);
      goto done;
    }
    //配置具体某一个源的参数
    if (!set_source_all_configs (config, cfg_file_path)) {
      ret = FALSE;
      goto done;
    }
    //从GkeyFile中移除前面添加的source-list组
    g_key_file_remove_group (cfg_file, CONFIG_GROUP_SOURCE_ALL, &error);
  }
//返回cfg_file中的所有的组
  groups = g_key_file_get_groups (cfg_file, NULL);
  for (group = groups; *group; group++) {
    gboolean parse_err = FALSE;
    GST_CAT_DEBUG (APP_CFG_PARSER_CAT, "Parsing group: %s", *group);
    //看第一组的组名是不是application。。。。这个具体的组名的顺序在deepstream配置文件中能看到。。
    if (!g_strcmp0 (*group, CONFIG_GROUP_APP)) {
      parse_err = !parse_app (config, cfg_file, cfg_file_path);
    }
    //找到source组
    if (!strncmp (*group, CONFIG_GROUP_SOURCE,
            sizeof (CONFIG_GROUP_SOURCE) - 1)) {
      //再次检查是否超出最大source数量的限制
      if (config->num_source_sub_bins == MAX_SOURCE_BINS) {
        NVGSTDS_ERR_MSG_V ("App supports max %d sources", MAX_SOURCE_BINS);
        ret = FALSE;
        goto done;
      }
      //初始化source组在所有组中的起始指针和结束指针
      gchar *source_id_start_ptr = *group + strlen (CONFIG_GROUP_SOURCE);
      gchar *source_id_end_ptr = NULL;
      //将字符串转换成gint索引
      guint index =
          g_ascii_strtoull (source_id_start_ptr, &source_id_end_ptr, 10);
      if (source_id_start_ptr == source_id_end_ptr
          || *source_id_end_ptr != '\0') {
        NVGSTDS_ERR_MSG_V
            ("Source group \"[%s]\" is not in the form \"[source<%%d>]\"",
            *group);
        ret = FALSE;
        goto done;
      }
      guint source_id = 0;
      //判断是否存在一组source。。就是很多数据源。。
      if (config->source_list_enabled) {
        //检查source索引是否超出范围
        if (index >= config->total_num_sources) {
          NVGSTDS_ERR_MSG_V
              ("Invalid source group index %d, index cannot exceed %d", index,
              config->total_num_sources);
          ret = FALSE;
          goto done;
        }
        //给某一个具体数据源的source的id赋值
        source_id = index;
        NVGSTDS_INFO_MSG_V ("Some parameters to be overwritten for group [%s]",
            *group);
      } else {
        //如果只有一个数据源。。id直接就是总数。。也就是0.。。毕竟初始化的时候是-1。。
        source_id = config->num_source_sub_bins;
      }
      parse_err = !parse_source (&config->multi_source_config[source_id],
          cfg_file, *group, cfg_file_path);
      if (config->source_list_enabled
          && config->multi_source_config[source_id].type ==
          NV_DS_SOURCE_URI_MULTIPLE) {
        NVGSTDS_ERR_MSG_V
            ("MultiURI support not available if [source-list] is provided");
        ret = FALSE;
        goto done;
      }
      //如果支持的类型正确，num_source_sub_bins加1，，可以看成是数据源的计数吧。。
      if (config->multi_source_config[source_id].enable
          && !config->source_list_enabled) {
        config->num_source_sub_bins++;
      }
    }
    //找到streammux组，并完成参数化
    if (!g_strcmp0 (*group, CONFIG_GROUP_STREAMMUX)) {
      parse_err = !parse_streammux (&config->streammux_config, cfg_file, cfg_file_path);
    }
    //找到osd组。。并完成参数化
    if (!g_strcmp0 (*group, CONFIG_GROUP_OSD)) {
      parse_err = !parse_osd (&config->osd_config, cfg_file);
    }
   //找到primary_gie组，完成参数化
    if (!g_strcmp0 (*group, CONFIG_GROUP_PRIMARY_GIE)) {
      parse_err =
          !parse_gie (&config->primary_gie_config, cfg_file,
          CONFIG_GROUP_PRIMARY_GIE, cfg_file_path);
    }
    //找到tracker组，完成参数化
    if (!g_strcmp0 (*group, CONFIG_GROUP_TRACKER)) {
      parse_err = !parse_tracker (&config->tracker_config, cfg_file, cfg_file_path);
    }
    //完成secondary_gie的参数化。。先判断是否存在这一组。。。
    if (!strncmp (*group, CONFIG_GROUP_SECONDARY_GIE,
                  sizeof (CONFIG_GROUP_SECONDARY_GIE) - 1)) {
      if (config->num_secondary_gie_sub_bins == MAX_SECONDARY_GIE_BINS) {
        NVGSTDS_ERR_MSG_V ("App supports max %d secondary GIEs", MAX_SECONDARY_GIE_BINS);
        ret = FALSE;
        goto done;
      }
      parse_err =
          !parse_gie (&config->secondary_gie_sub_bin_config[config->
                                  num_secondary_gie_sub_bins],
                                  cfg_file, *group, cfg_file_path);
      if (config->secondary_gie_sub_bin_config[config->num_secondary_gie_sub_bins].enable){
        config->num_secondary_gie_sub_bins++;
      }
    }
    //完成sink组的参数化。。跟source组差不多。主要是source组和sink组与其他组在数量上有差别。。可能存在多个[source]和[sink]
    if (!strncmp (*group, CONFIG_GROUP_SINK, sizeof (CONFIG_GROUP_SINK) - 1)) {
      if (config->num_sink_sub_bins == MAX_SINK_BINS) {
        NVGSTDS_ERR_MSG_V ("App supports max %d sinks", MAX_SINK_BINS);
        ret = FALSE;
        goto done;
      }
      //config->num_sink_sub_bins可以看成是对sink组件的计数
      parse_err =
          !parse_sink (&config->
          sink_bin_sub_bin_config[config->num_sink_sub_bins], cfg_file, *group,
          cfg_file_path);
      if (config->
          sink_bin_sub_bin_config[config->num_sink_sub_bins].enable){
        config->num_sink_sub_bins++;
      }
    }
    //找到message-consumer组。。完成参数化
    if (!strncmp (*group, CONFIG_GROUP_MSG_CONSUMER,
        sizeof (CONFIG_GROUP_MSG_CONSUMER) - 1)) {
      if (config->num_message_consumers == MAX_MESSAGE_CONSUMERS) {
        NVGSTDS_ERR_MSG_V ("App supports max %d consumers", MAX_MESSAGE_CONSUMERS);
        ret = FALSE;
        goto done;
      }
      parse_err = !parse_msgconsumer (
                    &config->message_consumer_config[config->num_message_consumers],
                    cfg_file, *group, cfg_file_path);

      if (config->message_consumer_config[config->num_message_consumers].enable) {
        config->num_message_consumers++;
      }
    }
    //找到tiled_display组，完成参数化配置
    if (!g_strcmp0 (*group, CONFIG_GROUP_TILED_DISPLAY)) {
      parse_err = !parse_tiled_display (&config->tiled_display_config, cfg_file);
    }
    //找到image-save组，完成参数化配置
    if (!g_strcmp0 (*group, CONFIG_GROUP_IMG_SAVE)) {
      parse_err = !parse_image_save (&config->image_save_config , cfg_file, *group, cfg_file_path);
    }
    //找到nvds-analytics组，完成参数化配置
    if (!g_strcmp0 (*group, CONFIG_GROUP_DSANALYTICS)) {
      parse_err = !parse_dsanalytics (&config->dsanalytics_config, cfg_file, cfg_file_path);
    }
    //找到ds-example组。。完成参数化配置
    if (!g_strcmp0 (*group, CONFIG_GROUP_DSEXAMPLE)) {
      parse_err = !parse_dsexample (&config->dsexample_config, cfg_file);
    }
    //找到message-converter组，完成参数化配置  
    if (!g_strcmp0 (*group, CONFIG_GROUP_MSG_CONVERTER)) {
      parse_err = !parse_msgconv (&config->msg_conv_config, cfg_file, *group, cfg_file_path);
    }
    //找到tests组，完成参数化配置
    if (!g_strcmp0 (*group, CONFIG_GROUP_TESTS)) {
      parse_err = !parse_tests (config, cfg_file);
    }

    if (parse_err) {
      GST_CAT_ERROR (APP_CFG_PARSER_CAT, "Failed to parse '%s' group", *group);
      goto done;
    }
  }
   //检查secondary-gie的相关序列id是否正确
  for (i = 0; i < config->num_secondary_gie_sub_bins; i++) {
    if (config->secondary_gie_sub_bin_config[i].unique_id ==
        config->primary_gie_config.unique_id) {
      NVGSTDS_ERR_MSG_V ("Non unique gie ids found");
      ret = FALSE;
      goto done;
    }
  }
//还是检查secondary-gie的序列id，主要是primary-gie和secondary-gie是一个嵌套的关系。。功能上先完成primary-gie的操作。。在后续完成secondary-gie的功能
  for (i = 0; i < config->num_secondary_gie_sub_bins; i++) {
    for (j = i + 1; j < config->num_secondary_gie_sub_bins; j++) {
      if (config->secondary_gie_sub_bin_config[i].unique_id ==
          config->secondary_gie_sub_bin_config[j].unique_id) {
        NVGSTDS_ERR_MSG_V ("Non unique gie id %d found",
                            config->secondary_gie_sub_bin_config[i].unique_id);
        ret = FALSE;
        goto done;
      }
    }
  }
//配置完成后最后再次检测相关信息是否正确，是否存在超出范围限制的操作，实现一源多用的copy
  for (i = 0; i < config->num_source_sub_bins; i++) {
    if (config->multi_source_config[i].type == NV_DS_SOURCE_URI_MULTIPLE) {
      if (config->multi_source_config[i].num_sources < 1) {
        config->multi_source_config[i].num_sources = 1;
      }
      //配置一源多用的操作。。就是deepstream的配置文件中的[source]组下面的num-sources
      for (j = 1; j < config->multi_source_config[i].num_sources; j++) {
        if (config->num_source_sub_bins == MAX_SOURCE_BINS) {
          NVGSTDS_ERR_MSG_V ("App supports max %d sources", MAX_SOURCE_BINS);
          ret = FALSE;
          goto done;
        }
        //实现一源多用的配置copy。。。
        memcpy (&config->multi_source_config[config->num_source_sub_bins],
            &config->multi_source_config[i],
            sizeof (config->multi_source_config[i]));
        config->multi_source_config[config->num_source_sub_bins].type =
            NV_DS_SOURCE_URI;
        config->multi_source_config[config->num_source_sub_bins].uri =
            g_strdup_printf (config->multi_source_config[config->
                num_source_sub_bins].uri, j);
        config->num_source_sub_bins++;
      }
      config->multi_source_config[i].type = NV_DS_SOURCE_URI;
      config->multi_source_config[i].uri =
          g_strdup_printf (config->multi_source_config[i].uri, 0);
    }
  }
  ret = TRUE;

done:
  if (cfg_file) {
    g_key_file_free (cfg_file);
  }

  if (groups) {
    g_strfreev (groups);
  }

  if (error) {
    g_error_free (error);
  }
  if (!ret) {
    NVGSTDS_ERR_MSG_V ("%s failed", __func__);
  }
  return ret;
}
