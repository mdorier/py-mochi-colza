import _colza_admin


class ColzaAdmin(_colza_admin.ColzaAdmin):

    def __init__(self, engine):
        super().__init__(engine.get_internal_mid())
