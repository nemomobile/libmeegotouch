/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of libdui.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

//============================================================================
// Description : Tool to handle notifications and notification groups
//============================================================================

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <getopt.h>
#include <DuiApplication>
#include <DuiNotification>
#include <DuiRemoteAction>
#include <DuiNotificationGroup>
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QApplication>
#include <duinotificationmanager.h>

// The actions for this tool
enum ToolAction {
    Undefined,
    Add,
    Update,
    Remove
};

// The action to perform
ToolAction toolAction = Undefined;

// Whether to operate on notification groups instead of notifications
bool groupMode = false;

// Persistent mode in use
bool persistentMode = false;

// Notifications list mode in use
bool listMode = false;

// Application name provided with list mode
QString applicationName("duinotificationtool");

// The notification/notification group ID to use
unsigned int id = 0;

// The notification/notification group ID to use
unsigned int count = 1;

// Prints usage information
int usage(const char *program)
{
    std::cerr << std::setw(7) << "Usage: " << program << " [OPTION]... EVENTTYPE [SUMMARY BODY IMAGEURI SERVICENAME OBJECTPATH INTERFACE METHODNAME ARG...]" << std::endl;
    std::cerr << std::setw(7) << "Manage notifications." << std::endl;
    std::cerr << std::setw(7) << std::endl;
    std::cerr << std::setw(7) << "Mandatory arguments to long options are mandatory for short options too." << std::endl;
    std::cerr << std::setw(7) << "  -a, --action=ACTION        The action (add/update/remove) to perform." << std::endl;
    std::cerr << std::setw(7) << "                             add - Adds a new notification or notification group." << std::endl;
    std::cerr << std::setw(7) << "                             update - Updates an existing notification or notification group." << std::endl;
    std::cerr << std::setw(7) << "                             remove - Removes an existing notification or notification group." << std::endl;
    std::cerr << std::setw(7) << "  -g, --group                Whether to operate on notification groups instead of notifications." << std::endl;
    std::cerr << std::setw(7) << "  -i, --id=ID                The notification/notification group ID to use." << std::endl;
    std::cerr << std::setw(7) << "  -c, --count=NUMBER         The number of notifications. This parameter has no effect when the action is 'remove'" << std::endl;
    std::cerr << std::setw(7) << "  -p, --persistent           Set notification/group to persistent. This parameter has no effect when the action is 'remove'" << std::endl;
    std::cerr << std::setw(7) << "  -l, --list=APPLICATION     List all notifications that belong to an application name. Returns a count of notifications as an exit value." << std::endl;
    std::cerr << std::setw(7) << "      --help     display this help and exit" << std::endl;
    return -1;
}

// Parses command line arguments (the flags)
int parseArguments(int argc, char *argv[])
{
    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            { "action", 1, NULL, 'a' },
            { "group", 0, NULL, 'g' },
            { "id", 1, NULL, 'i' },
            { "count", 1, NULL, 'c' },
            { "help", 0, NULL, 'h' },
            { "list", 1, NULL, 'l'},
            { "persistent", 0, NULL, 'p'},
            { 0, 0, 0, 0 }
        };

        int c = getopt_long(argc, argv, "a:gi:c:pl:", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 'a':
            if (strcmp(optarg, "add") == 0) {
                toolAction = Add;
            } else if (strcmp(optarg, "update") == 0) {
                toolAction = Update;
            } else if (strcmp(optarg, "remove") == 0) {
                toolAction = Remove;
            }
            break;
        case 'i':
            id = atoi(optarg);
            break;
        case 'c':
            count = atoi(optarg);
            break;
        case 'g':
            groupMode = true;
            break;
        case 'p':
            persistentMode = true;
            break;
        case 'l':
            listMode = true;
            applicationName = QString(optarg);
            break;
        case 'h':
            return usage(argv[0]);
        default:
            break;
        }
    }

    if (!listMode) {
        if (toolAction == Undefined ||
                (toolAction == Add && argc < optind + 1) ||
                (toolAction == Update && argc < optind + 4) ||
                ((toolAction == Update || toolAction == Remove) && id == 0)) {
            return usage(argv[0]);
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    // Parse arguments
    int result = parseArguments(argc, argv);
    if (result != 0) {
        return result;
    }

    // DuiApplication creates DuiNotificationManager for specified application
    QScopedPointer<DuiApplication> duiApplication(new DuiApplication(argc, argv, applicationName, 0));
    if (duiApplication.isNull()) {
        std::cerr << "Couldn't initialize DuiApplication" << std::endl;
        return -1;
    }

    // Calls notificationIdList from NotificationManager. Returns size of the list from main.
    if (listMode) {
        QList<DuiNotification *> list = DuiNotification::notifications();
        std::cout << "\n" << applicationName.toUtf8().data() << " has " << list.size() << " notifications." << std::endl;
        std::cout << "Notification id(s):" << std::endl;
        foreach(DuiNotification * notification, list) {
            std::cout << notification->id() << std::endl;
        }
        return list.size();
    }

    // Execute the desired action
    switch (toolAction) {
    case Add:
    case Update: {
        // Get the parameters for adding and updating notifications/notification groups
        QString eventType = QString(argv[optind]);
        QString summary, body, imageURI;
        if (argc >= optind + 1) {
            summary = QString(argv[optind + 1]);
        }
        if (argc >= optind + 2) {
            body = QString(argv[optind + 2]);
        }
        if (argc >= optind + 3) {
            imageURI = QString(argv[optind + 3]);
        }

        // Create an action if one is defined
        DuiRemoteAction *remoteAction;
        if (argc >= optind + 4 + 4) {
            QString serviceName(argv[optind + 4]);
            QString objectPath(argv[optind + 5]);
            QString interface(argv[optind + 6]);
            QString methodName(argv[optind + 7]);
            QList<QVariant> arguments;

            for (int i = optind + 8; i < argc; ++i) {
                arguments.append(QVariant(argv[i]));
            }
            remoteAction = new DuiRemoteAction(serviceName, objectPath, interface, methodName, arguments);
        } else {
            remoteAction = new DuiRemoteAction;
        }

        if (toolAction == Add) {
            // Add a notification/notification group
            if (groupMode) {
                result = DuiNotificationGroup(eventType, summary, body, imageURI, *remoteAction, count, persistentMode).id();
            } else {
                if (id != 0) {
                    result = DuiNotification(DuiNotificationGroup(id), eventType, summary, body, imageURI, *remoteAction).id();
                } else {
                    result = DuiNotification(eventType, summary, body, imageURI, *remoteAction, count, persistentMode).id();
                }
            }
        } else {
            // Update a notification/notification group
            if (groupMode) {
                DuiNotificationGroup group(id);
                group.update(eventType, summary, body, imageURI, count, *remoteAction);
            } else {
                DuiNotification notification(id);
                notification.update(eventType, summary, body, imageURI, count, *remoteAction);
            }
        }

        delete remoteAction;
        break;
    }
    case Remove:
        if (groupMode) {
            DuiNotificationGroup group(id);
            group.remove();
        } else {
            DuiNotification notification(id);
            notification.remove();
        }
        break;
    default:
        break;
    }

    return result;
}
