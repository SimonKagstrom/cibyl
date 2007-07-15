    private static int AlertType_INFO_handle = 0;
    private static int AlertType_WARNING_handle = 0;
    private static int AlertType_ERROR_handle = 0;
    private static int AlertType_ALARM_handle = 0;
    private static int AlertType_CONFIRMATION_handle = 0;

    public static final int NOPH_AlertType_get(int type)
    {
        switch (type) {
            case 2:
                if (AlertType_WARNING_handle == 0)
                    AlertType_WARNING_handle =
                        CRunTime.registerObject(AlertType.WARNING);
                return AlertType_WARNING_handle;
            case 3:
                if (AlertType_ERROR_handle == 0)
                    AlertType_ERROR_handle =
                        CRunTime.registerObject(AlertType.ERROR);
                return AlertType_ERROR_handle;
            case 4:
                if (AlertType_ALARM_handle == 0)
                    AlertType_ALARM_handle =
                        CRunTime.registerObject(AlertType.ALARM);
                return AlertType_ALARM_handle;
            case 5:
                if (AlertType_CONFIRMATION_handle == 0)
                    AlertType_CONFIRMATION_handle =
                        CRunTime.registerObject(AlertType.CONFIRMATION);
                return AlertType_CONFIRMATION_handle;
            default:
                if (AlertType_INFO_handle == 0)
                    AlertType_INFO_handle =
                        CRunTime.registerObject(AlertType.INFO);
                return AlertType_INFO_handle;
        }
    }

