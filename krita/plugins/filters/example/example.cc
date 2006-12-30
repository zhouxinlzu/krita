/*
 * This file is part of the KDE project
 *
 * Copyright (c) 2004 Cyrille Berger <cberger@cberger.net>
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <stdlib.h>
#include <vector>

#include <QPoint>

#include <klocale.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kgenericfactory.h>

#include <kis_image.h>
#include <kis_iterators_pixel.h>
#include <kis_layer.h>
#include <kis_filter_registry.h>
#include <kis_global.h>
#include <kis_types.h>

// #include <kmessagebox.h>

#include "example.h"

typedef KGenericFactory<KritaExample> KritaExampleFactory;
K_EXPORT_COMPONENT_FACTORY( kritaexample, KritaExampleFactory( "krita" ) )

KritaExample::KritaExample(QObject *parent, const QStringList &)
        : KParts::Plugin(parent)
{
    setInstance(KritaExampleFactory::instance());


    if (parent->inherits("KisFilterRegistry")) {
        KisFilterRegistry * manager = dynamic_cast<KisFilterRegistry *>(parent);
        manager->add(KisFilterSP(new KisFilterInvert()));
    }
}

KritaExample::~KritaExample()
{
}

KisFilterInvert::KisFilterInvert() : KisFilter(id(), "adjust", i18n("&Invert"))
{
}

void KisFilterInvert::process(const KisPaintDeviceSP src, const QPoint& srcTopLeft, KisPaintDeviceSP dst, const QPoint& dstTopLeft, const QSize& size, KisFilterConfiguration* /*config*/)
{
    Q_ASSERT(!src.isNull());
    Q_ASSERT(!dst.isNull());

    KisRectIteratorPixel dstIt = dst->createRectIterator(dstTopLeft.x(), dstTopLeft.y(), size.width(), size.height());
    KisRectConstIteratorPixel srcIt = src->createRectConstIterator(srcTopLeft.x(), srcTopLeft.y(), size.width(), size.height());

    int pixelsProcessed = 0;
    setProgressTotalSteps(size.width() * size.height());

    KoColorSpace * cs = src->colorSpace();

    KoColorTransformation* inverter = cs->createInvertTransformation();
    while( ! srcIt.isDone() )
    {
        if(srcIt.isSelected())
        {
            inverter->transform( srcIt.oldRawData(), dstIt.rawData(), 1);
        }
        setProgress(++pixelsProcessed);
        ++srcIt;
        ++dstIt;
    }
    delete inverter;
    setProgressDone(); // Must be called even if you don't really support progression
}
