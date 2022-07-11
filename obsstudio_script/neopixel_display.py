from concurrent.futures import ThreadPoolExecutor
from multiprocessing import SimpleQueue
import obspython as obs
import asyncio
import json
import urllib.request
import urllib.error
import urllib.parse
import sys

#     ____                _              _       
#    / ___|___  _ __  ___| |_ __ _ _ __ | |_ ___ 
#   | |   / _ \| '_ \/ __| __/ _` | '_ \| __/ __|
#   | |__| (_) | | | \__ \ || (_| | | | | |_\__ \
#    \____\___/|_| |_|___/\__\__,_|_| |_|\__|___/
#                                                

# Output control options
# Specify output action (no action, on, or off)
# 
# Used in 2 character array in COLOR_INDEX_OPTIONS value:
#    Upon Activate, Upon Deactivate
OUT_NA = ' '    # No action
OUT_OFF = '0'   # Turn output / LED off
OUT_ON = '1'    # Turn output / LED on
OUT_BLINK = '2' # Blink LED

# output entity names: tuples of tuples: Array index, string
OUTPUT_NAMES = (('0', 'Audio'), ('1', 'Relay 1'), ('2', 'Relay 2'), ('3', 'Relay 3'),
  ('4', 'Relay 4'), ('5', 'Relay 5'))

LED_NAMES =  (('0', 'LED 1'), ('1', 'LED 2'), ('2', 'LED 3'), ('3', 'LED 4'))

# Option lists: tuples of tuples: Value, Name
COLOR_INDEX_OPTIONS =  (('-1', 'No Action'), ('0', 'off'), ('1', 'Rainbow'), 
  ('2', 'Red'), ('3', 'Orange'), ('4', 'Yellow'), 
  ('5', 'Green'), ('6', 'Blue'), ('7', 'Indigo'), 
  ('8', 'Violet'))

OUTPUT_ENABLE_OPTIONS = ((OUT_NA + OUT_NA, 'No Action'), (OUT_ON + OUT_NA, 'Active=ON'),
  (OUT_OFF + OUT_NA, 'Active=OFF'), ( OUT_NA + OUT_ON, 'Inactive=ON'), 
  ( OUT_NA + OUT_OFF, 'Inactive=OFF'), (OUT_ON + OUT_OFF, 'Active=ON Inactive=OFF'),  
  (OUT_OFF + OUT_ON, 'Active=OFF Inactive=ON'))

LED_ENABLE_OPTIONS = ((OUT_NA + OUT_NA, 'No Action'),
  (OUT_ON + OUT_NA, 'Active=ON'),  (OUT_OFF + OUT_NA, 'Active=OFF'), ( OUT_BLINK + OUT_OFF, 'Active=BLINK'),
  (OUT_NA + OUT_ON, 'Inactive=ON'),  (OUT_NA + OUT_OFF, 'Inactive=OFF'), ( OUT_NA + OUT_BLINK, 'Inactive=BLINK'),
  (OUT_ON + OUT_OFF, 'Active=ON Inactive=OFF'),  (OUT_BLINK + OUT_OFF, 'Active=BLINK Inactive=OFF'),
  (OUT_OFF + OUT_ON, 'Active=OFF Inactive=ON'),  (OUT_OFF + OUT_BLINK, 'Active=OFF Inactive=BLINK'))


#     ____ _       _           _     
#    / ___| | ___ | |__   __ _| |___ 
#   | |  _| |/ _ \| '_ \ / _` | / __|
#   | |_| | | (_) | |_) | (_| | \__ \
#    \____|_|\___/|_.__/ \__,_|_|___/
#                                
#     
threadpool = ThreadPoolExecutor(max_workers=1)
messageQueue = SimpleQueue()
neopixle_ip = ''
action_mapping = {}
http_timeout_seconds = 4

program_name = ''
perview_name = ''

def script_description():
    return "NEO Pixel Banner Display."


#    ____       _   _   _
#   / ___|  ___| |_| |_(_)_ __   __ _ ___
#   \___ \ / _ \ __| __| | '_ \ / _` / __|
#    ___) |  __/ |_| |_| | | | | (_| \__ \
#   |____/ \___|\__|\__|_|_| |_|\__, |___/
#                               |___/

def settings_dict(settings):
    """ Fetch settings as JSON object to simplify processing """
    settings_json = obs.obs_data_get_json(settings)
    # print(settings_json)
    return json.loads(settings_json)


def script_defaults(settings):
    """ OBS-API Set initial (and default) property values """
    source_names = list_scene_names()
    for source_name in source_names:
        scene_defaults(settings, source_name)

    source_names = list_source_names()
    for source_name in source_names:
        scene_defaults(settings, source_name)


def script_update(settings):
    """ OBS-API Retrieve settings (modified by properties UI) and save as global variables """
    global neopixle_ip
    global action_mapping

    source_names = list_source_names()
    settings_map = settings_dict(settings)
    # print(settings_map)
    action_mapping = {}
    for k, v in settings_map.items():
        if k[0:4] != "neo^":
            # print("mapping:" + k + ", " + v)
            obj = {}
            parts = k.split('^')
            if parts[0] in action_mapping:
                obj = action_mapping[parts[0]]
                obj[parts[1]] = v
                # obs.script_log(obs.LOG_DEBUG, 'Loaded: %s=%s' % (k, obj))
            else:
                obj[parts[1]] = v
                action_mapping[parts[0]] = obj

    neopixle_ip = obs.obs_data_get_string(settings, "neo^ip")

#    ____                            _   _
#   |  _ \ _ __ ___  _ __   ___ _ __| |_(_) ___  ___
#   | |_) | '__/ _ \| '_ \ / _ \ '__| __| |/ _ \/ __|
#   |  __/| | | (_) | |_) |  __/ |  | |_| |  __/\__ \
#   |_|   |_|  \___/| .__/ \___|_|   \__|_|\___||___/
#                   |_|

def script_properties():
    """ OBS-API Create and display properties UI - connected to settings """
    props=obs.obs_properties_create()

    obs.obs_properties_add_text(
        props,  "neo^ip", "NeoPixel Display IP", obs.OBS_TEXT_DEFAULT)

    source_names=list_scene_names()
    for source_name in source_names:
        action_properties_create(props, source_name, "Scene")

    source_names=list_source_names()
    for source_name in source_names:
        action_properties_create(props, source_name, "Source")

    return props

def scene_defaults(settings, name):
    obs.obs_data_set_default_int(
        settings, name + "^color", int('ffcccc00', 16))

#     ___      __  _             ___                        __  _       
#    / _ |____/ /_(_)__  ___    / _ \_______  ___  ___ ____/ /_(_)__ ___
#   / __ / __/ __/ / _ \/ _ \  / ___/ __/ _ \/ _ \/ -_) __/ __/ / -_|_-<
#  /_/ |_\__/\__/_/\___/_//_/ /_/  /_/  \___/ .__/\__/_/  \__/_/\__/___/
#                                          /_/                          

def action_properties_create(props, name, category):
    enabled = False
    props_scene = obs.obs_properties_create()
    if name in action_mapping:
        obj = action_mapping[name]
        if 'enabled' in obj and obj['enabled']:
            enabled = True
    textaction = obs.obs_properties_add_list(props_scene, name + "^textaction", "Text color \n/ enable", obs.OBS_COMBO_TYPE_LIST , obs.OBS_COMBO_FORMAT_STRING )
    options_add(textaction, COLOR_INDEX_OPTIONS)

    obs.obs_properties_add_text(
        props_scene, name + "^text", "Display Text", obs.OBS_TEXT_DEFAULT)
    obs.obs_properties_add_bool(
        props_scene, name + "^textoff", "Text off upon deactiviate")
    #obs.obs_properties_add_color(props_scene, name + "^color", "Color")

    ledaction = obs.obs_properties_add_list(props_scene, name + "^ledaction", "LED Action", obs.OBS_COMBO_TYPE_LIST , obs.OBS_COMBO_FORMAT_STRING )
    options_add(ledaction, LED_ENABLE_OPTIONS)
    leds = obs.obs_properties_add_list(
        props_scene, name + "^leds", "LEDS", obs.OBS_COMBO_TYPE_LIST , obs.OBS_COMBO_FORMAT_STRING )
    options_add(leds, LED_NAMES)

    outputenable = obs.obs_properties_add_list(props_scene, name + "^outputenable", "I/O Action", obs.OBS_COMBO_TYPE_LIST , obs.OBS_COMBO_FORMAT_STRING )
    options_add(outputenable, OUTPUT_ENABLE_OPTIONS)
    outputs = obs.obs_properties_add_list(
        props_scene, name + "^outputs", "Physical IO", obs.OBS_COMBO_TYPE_LIST , obs.OBS_COMBO_FORMAT_STRING )
    options_add(outputs, OUTPUT_NAMES)

    group = obs.obs_properties_add_group(props, name + "^enabled", "'" + name + "' "
    + category + ' (script enable)', obs.OBS_GROUP_CHECKABLE, props_scene)

    # print("Prop: " + name + " enabled=" + str(enabled) )
    if not enabled:
        action_properties_enable(False, props, name)

    obs.obs_property_set_modified_callback(group, action_enabled_callback)

def options_add(prop, options):
    for option in options:
        obs.obs_property_list_add_string(prop, option[1], option[0])


def action_properties_enable(enabled, props, source):
    obs.obs_property_set_visible(
        obs.obs_properties_get(props, source + '^text'), enabled)
    obs.obs_property_set_visible(
        obs.obs_properties_get(props, source + '^textoff'), enabled)
    obs.obs_property_set_visible(
        obs.obs_properties_get(props, source + '^textaction'), enabled)
    obs.obs_property_set_visible(
        obs.obs_properties_get(props, source + '^ledaction'), enabled)
    obs.obs_property_set_visible(
        obs.obs_properties_get(props, source + '^leds'), enabled)
    obs.obs_property_set_visible(
        obs.obs_properties_get(props, source + '^outputenable'), enabled)
    obs.obs_property_set_visible(
        obs.obs_properties_get(props, source + '^outputs'), enabled)

def action_properties_exec(scene_name, isPreview, activate):
    # todo - For now, ignore iPreview and activate. 
    # Only need to worry about it if the UI gives the user the abality to 
    # configure them
    #obs.script_log(obs.LOG_INFO, 'Processing %s Scene [%s]' % ('PREVIEW' if isPreview is True else 'PROGRAM', scene_name))
    if scene_name in action_mapping:
        changeCount = 0
        action_index = 1
        if activate:
            action_index = 0
        obj=action_mapping[scene_name]
        #obs.script_log(obs.LOG_INFO, 'Scene [%s]: %s' % (scene_name, obj))
        if 'enabled' in obj:
            if obj['enabled']:
                changes = {}
                if activate and 'textaction' in obj:
                    textaction = int(obj['textaction'])
                    if textaction >= 0:
                        # NOT ;No Action'
                        changeCount += 1
                        text = ''
                        if 'text' in obj:    
                            text = obj['text']
                        changes['text'] = text
                        changes['txtclr'] = textaction
                if not activate and 'textoff' in obj:
                    action = bool(obj['textoff'])
                    if action:
                        changes['txtclr'] = '0'
                if 'ledaction' in obj:
                    ledaction = obj['ledaction']
                    if ledaction[action_index] != OUT_NA:
                        ledstate = ledaction[action_index]
                        changeCount += 1
                        led = '0'
                        if 'leds' in obj:    
                            led = obj['leds']
                        changes['leds[' + led + ']'] = ledstate
                if 'outputenable' in obj:
                    outputenable = obj['outputenable']
                    if outputenable[action_index] != OUT_NA:
                        outputstate = outputenable[action_index]
                        changeCount += 1
                        output = '0'
                        if 'outputs' in obj:    
                            output = obj['output']
                        changes['outputs[' + output + ']'] = outputstate

                if changeCount > 0:
                    device_submit_change(changes)
                else:
                    print('No Changes')

def action_enabled_callback(props, prop, settings):
    name = obs.obs_property_name(prop)
    parts = name.split('^')
    source = parts[0]
    action_properties_enable(obs.obs_data_get_bool(settings, name), props, source)
    return True

#     ___  ____ ____     __                 _                 _ 
#    / _ \| __ ) ___|   / _|_ __ ___  _ __ | |_ ___ _ __   __| |
#   | | | |  _ \___ \  | |_| '__/ _ \| '_ \| __/ _ \ '_ \ / _` |
#   | |_| | |_) |__) | |  _| | | (_) | | | | ||  __/ | | | (_| |
#    \___/|____/____/  |_| |_|  \___/|_| |_|\__\___|_| |_|\__,_|
#                                                               

def list_scene_names():
    """ Make a list of all obs scenes """
    sources=obs.obs_frontend_get_scenes()
    source_names=[]

    if sources is not None:
        for source in sources:
            # source_id = obs.obs_source_get_id(source)
            source_name=obs.obs_source_get_name(source)
            source_names.append(source_name)

    obs.source_list_release(sources)
    return source_names

def list_source_names():
    """ Make a list of all obs sources """
    sources=obs.obs_enum_sources()
    source_names=[]

    if sources is not None:
        for source in sources:
            # source_id = obs.obs_source_get_id(source)
            source_name=obs.obs_source_get_name(source)
            source_names.append(source_name)

    obs.source_list_release(sources)
    return source_names

def get_active_sources_by_scene(source):
    item_names=[]
    scene=obs.obs_scene_from_source(source)
    scene_name=obs.obs_source_get_name(source)
    scene_items=obs.obs_scene_enum_items(scene)
    if scene_items is not None:
        for item in scene_items:
            item_source=obs.obs_sceneitem_get_source(item)
            if obs.obs_source_active(item_source):
                item_name=obs.obs_source_get_name(item_source)
                if item_name in action_mapping:
                    #print('Scene item: ', item_name)
                    item_names.append(item_name)
        obs.sceneitem_list_release(scene_items)

        return item_names

#    ____            _       _     ____              _   _
#   / ___|  ___ _ __(_)_ __ | |_  |  _ \ _   _ _ __ | |_(_)_ __ ___   ___
#   \___ \ / __| '__| | '_ \| __| | |_) | | | | '_ \| __| | '_ ` _ \ / _ \
#    ___) | (__| |  | | |_) | |_  |  _ <| |_| | | | | |_| | | | | | |  __/
#   |____/ \___|_|  |_| .__/ \__| |_| \_\\__,_|_| |_|\__|_|_| |_| |_|\___|
#                     |_|

def script_load(settings):
    """ OBS-API script startup """
    global preview_name
    global program_name

    preview_source=obs.obs_frontend_get_current_preview_scene()
    preview_name=obs.obs_source_get_name(preview_source)
    program_source=obs.obs_frontend_get_current_scene()
    program_name = obs.obs_source_get_name(program_source)

    obs.obs_frontend_add_event_callback(handle_event)


def script_unload():
    """ OBS-API script shutdown """
    print('shutdown')
    threadpool.shutdown()


#     ____              __    __                ____          
#    / __/  _____ ___  / /_  / /  ___ ____  ___/ / /__ _______
#   / _/| |/ / -_) _ \/ __/ / _ \/ _ `/ _ \/ _  / / -_) __(_-<
#  /___/|___/\__/_//_/\__/ /_//_/\_,_/_//_/\_,_/_/\__/_/ /___/
#                                                             

def handle_event(event):
    if event is obs.OBS_FRONTEND_EVENT_SCENE_CHANGED:
        handle_program_change()
    elif event is obs.OBS_FRONTEND_EVENT_PREVIEW_SCENE_CHANGED:
        handle_preview_change()

def handle_preview_change():
    global preview_name
    global preview_items

    #program_source=obs.obs_frontend_get_current_scene()
    #program_name=obs.obs_source_get_name(program_source)
    #obs.obs_source_release(program_source)

    action_properties_exec(preview_name, True, False)
    preview_source=obs.obs_frontend_get_current_preview_scene()
    preview_name=obs.obs_source_get_name(preview_source)
    if preview_name:
        preview_items=get_active_sources_by_scene(preview_source)
        # if program_name != preview_name:
        # todo Hard coded - preview, activate
        action_properties_exec(preview_name, True, True)

    obs.obs_source_release(preview_source)

def handle_program_change():
    global program_name
    global program_items

    action_properties_exec(program_name, False, False)

    program_source=obs.obs_frontend_get_current_scene()
    program_name = obs.obs_source_get_name(program_source)
    program_items=get_active_sources_by_scene(program_source)
    #for item in scene_items:
    #    if item.
    #print(program_items)
    #todo We need to do something about soruces (program_items) ??
    # todo Hard coded - Not preview, activate
    action_properties_exec(program_name, False, True)
    obs.obs_source_release(program_source)

#    _   _            ____  _          _   ____             _
#   | \ | | ___  ___ |  _ \(_)_  _____| | |  _ \  _____   _(_) ___ ___
#   |  \| |/ _ \/ _ \| |_) | \ \/ / _ \ | | | | |/ _ \ \ / / |/ __/ _ \
#   | |\  |  __/ (_) |  __/| |>  <  __/ | | |_| |  __/\ V /| | (_|  __/
#   |_| \_|\___|\___/|_|   |_/_/\_\___|_| |____/ \___| \_/ |_|\___\___|
#

def device_submit_change(changes):
    """ Send a 'display banner' message to the NeoPixel Display """
    messageQueue.put(changes);
    threadpool.submit(device_commit_change)

def device_commit_change():
    """ Threaded function - process next item in message queue """
    params=messageQueue.get()
    url='http://' + neopixle_ip + \
        '/status?' + urllib.parse.urlencode(params)
    #print(url)
    try:
        with urllib.request.urlopen(url, None, http_timeout_seconds) as response:
            data=response.read()
            text=data.decode('utf-8')
            #obs.script_log(obs.LOG_INFO, 'Sent: ' + url)

    except urllib.error.URLError as err:
        obs.script_log(obs.LOG_WARNING, 'Error connecting to neopixel banner')

