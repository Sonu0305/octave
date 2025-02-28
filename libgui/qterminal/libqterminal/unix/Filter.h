/*
    Copyright (C) 2007, 2013 by Robert Knight <robertknight@gmail.com>

    Rewritten for QT4 by e_k <e_k at users.sourceforge.net>, Copyright (C)2008
    Adoption to octave by Torsten <mttl@mailbox.org>, Copyright (c) 2017

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301  USA.
*/

#ifndef FILTER_H
#define FILTER_H

// Qt
#include <QAction>
#include <QHash>
#include <QList>
#include <QObject>
#include <QRegularExpression>
#include <QStringList>

// Local
#include "unix/Character.h"

/**
 * A filter processes blocks of text looking for certain patterns (such as URLs or keywords from a list)
 * and marks the areas which match the filter's patterns as 'hotspots'.
 *
 * Each hotspot has a type identifier associated with it ( such as a link or a highlighted section ),
 * and an action.  When the user performs some activity such as a mouse-click in a hotspot area ( the exact
 * action will depend on what is displaying the block of text which the filter is processing ), the hotspot's
 * activate() method should be called.  Depending on the type of hotspot this will trigger a suitable response.
 *
 * For example, if a hotspot represents a URL then a suitable action would be opening that URL in a web browser.
 * Hotspots may have more than one action, in which case the list of actions can be obtained using the
 * actions() method.
 *
 * Different subclasses of filter will return different types of hotspot.
 * Subclasses must reimplement the process() method to examine a block of text and identify sections of interest.
 * When processing the text they should create instances of Filter::HotSpot subclasses for sections of interest
 * and add them to the filter's list of hotspots using addHotSpot()
 */
class Filter : public QObject
{

   Q_OBJECT

public:
    /**
    * Represents an area of text which matched the pattern a particular filter has been looking for.
    *
    * Each hotspot has a type identifier associated with it ( such as a link or a highlighted section ),
    * and an action.  When the user performs some activity such as a mouse-click in a hotspot area ( the exact
    * action will depend on what is displaying the block of text which the filter is processing ), the hotspot's
    * activate() method should be called.  Depending on the type of hotspot this will trigger a suitable response.
    *
    * For example, if a hotspot represents a URL then a suitable action would be opening that URL in a web browser.
    * Hotspots may have more than one action, in which case the list of actions can be obtained using the
    * actions() method.  These actions may then be displayed in a popup menu or toolbar for example.
    */

    enum Type
    {
      // the type of the hotspot is not specified
      NotSpecified,
      // this hotspot represents a clickable link
      Link,
      // this hotspot represents a marker
      Marker,
      // this hotspot represents a clickable link to an erroneous file
      ErrorLink
    };

    class HotSpot : public QObject
    {

    public:
       /**
        * Constructs a new hotspot which covers the area from (@p startLine,@p startColumn) to (@p endLine,@p endColumn)
        * in a block of text.
        */
       HotSpot(int startLine , int startColumn , int endLine , int endColumn);
       virtual ~HotSpot();

       /** Returns the line when the hotspot area starts */
       int startLine() const;
       /** Returns the line where the hotspot area ends */
       int endLine() const;
       /** Returns the column on startLine() where the hotspot area starts */
       int startColumn() const;
       /** Returns the column on endLine() where the hotspot area ends */
       int endColumn() const;
       /**
        * Returns the type of the hotspot.  This is usually used as a hint for views on how to represent
        * the hotspot graphically.  eg.  Link hotspots are typically underlined when the user mouses over them
        */
       Type type() const;
       /**
        * Causes the an action associated with a hotspot to be triggered.
        *
        * @param object The object which caused the hotspot to be triggered.  This is
        * typically null ( in which case the default action should be performed ) or
        * one of the objects from the actions() list.  In which case the associated
        * action should be performed.
        */
       virtual void activate(QObject* object = nullptr) = 0;
       /**
        * Returns a list of actions associated with the hotspot which can be used in a
        * menu or toolbar
        */
       virtual QList<QAction*> actions();

       /**
        * Returns the text of a tooltip to be shown when the mouse moves over the hotspot, or
        * an empty string if there is no tooltip associated with this hotspot.
        *
        * The default implementation returns an empty string.
        */
       virtual QString tooltip() const;

    protected:
       /** Sets the type of a hotspot.  This should only be set once */
       void setType(Type type);

    private:
       int    _startLine;
       int    _startColumn;
       int    _endLine;
       int    _endColumn;
       Type _type;

    };

    /** Constructs a new filter. */
    Filter();
    virtual ~Filter();

    /** Causes the filter to process the block of text currently in its internal buffer */
    virtual void process() = 0;

    /**
     * Empties the filters internal buffer and resets the line count back to 0.
     * All hotspots are deleted.
     */
    void reset();

    /** Adds a new line of text to the filter and increments the line count */
    //void addLine(const QString& string);

    /** Returns the hotspot which covers the given @p line and @p column, or 0 if no hotspot covers that area */
    HotSpot* hotSpotAt(int line , int column) const;

    /** Returns the list of hotspots identified by the filter */
    QList<HotSpot*> hotSpots() const;

    /** Returns the list of hotspots identified by the filter which occur on a given line */
    QList<HotSpot*> hotSpotsAtLine(int line) const;

    /**
     * TODO: Document me
     */
    void setBuffer(const QString* buffer , const QList<int>* linePositions);

protected:
    /** Adds a new hotspot to the list */
    void addHotSpot(HotSpot*);
    /** Returns the internal buffer */
    const QString* buffer();
    /** Converts a character position within buffer() to a line and column */
    void getLineColumn(int position , int& startLine , int& startColumn);

private:
    QMultiHash<int,HotSpot*> _hotspots;
    QList<HotSpot*> _hotspotList;

    const QList<int>* _linePositions;
    const QString* _buffer;
};

/**
 * A filter which searches for sections of text matching a regular expression and creates a new RegExpFilter::HotSpot
 * instance for them.
 *
 * Subclasses can reimplement newHotSpot() to return custom hotspot types when matches for the regular expression
 * are found.
 */
class RegExpFilter : public Filter
{

   Q_OBJECT

public:
    /**
     * Type of hotspot created by RegExpFilter.  The capturedTexts() method can be used to find the text
     * matched by the filter's regular expression.
     */
    class HotSpot : public Filter::HotSpot
    {
    public:
        HotSpot(int startLine, int startColumn,
                int endLine , int endColumn, Filter::Type);
        virtual void activate(QObject* object = nullptr);

        /** Sets the captured texts associated with this hotspot */
        void setCapturedTexts(const QStringList& texts);
        /** Returns the texts found by the filter when matching the filter's regular expression */
        QStringList capturedTexts() const;
     private:
        QStringList _capturedTexts;
    };

    /** Constructs a new regular expression filter */
    RegExpFilter (Type);

    /**
     * Sets the regular expression which the filter searches for in blocks of text.
     *
     * Regular expressions which match the empty string are treated as not matching
     * anything.
     */
    void setRegExp(const QRegularExpression& text);
    /** Returns the regular expression which the filter searches for in blocks of text */
    QRegularExpression regExp() const;

    /**
     * Reimplemented to search the filter's text buffer for text matching regExp()
     *
     * If regexp matches the empty string, then process() will return immediately
     * without finding results.
     */
    virtual void process();

Q_SIGNALS:

    void request_edit_mfile_signal (const QString&, int);
    void request_open_file_signal (const QString&, const QString&, int);

protected:
    /**
     * Called when a match for the regular expression is encountered.  Subclasses should reimplement this
     * to return custom hotspot types
     */
    virtual RegExpFilter::HotSpot* newHotSpot(int startLine,int startColumn,
                                    int endLine,int endColumn, Type);
    Type _type;

    QRegularExpression _searchText;
};

class FilterObject;

/** A filter which matches URLs in blocks of text */
class UrlFilter : public RegExpFilter
{

   Q_OBJECT

public:
    /**
     * Hotspot type created by UrlFilter instances.  The activate() method opens a web browser
     * at the given URL when called.
     */
    class HotSpot : public RegExpFilter::HotSpot
    {
    public:
        HotSpot(int startLine,int startColumn,int endLine,int endColumn,Type t);
        virtual ~HotSpot();

        virtual QList<QAction*> actions();

        /**
         * Open a web browser at the current URL.  The url itself can be determined using
         * the capturedTexts() method.
         */
        virtual void activate(QObject* object = nullptr);

        virtual QString tooltip() const;

        FilterObject* get_urlObject () { return _urlObject; }

    private:
        enum UrlType
        {
            StandardUrl,
            Email,
            ErrorLink,
            ParseErrorLink,
            Unknown
        };
        UrlType urlType() const;

        FilterObject* _urlObject;
    };

    UrlFilter (Type t = Link);

    virtual void process();

public Q_SLOTS:
    void request_open_file (const QString&, int);

protected:
    virtual HotSpot* newHotSpot(int,int,int,int,Type);

private:

    static const QRegularExpression FullUrlRegExp;
    static const QRegularExpression EmailAddressRegExp;
    static const QRegularExpression ErrorLinkRegExp;
    static const QRegularExpression ParseErrorLinkRegExp;
    static const QRegularExpression CompleteErrorLinkRegExp;

    // combined OR of FullUrlRegExp and EmailAddressRegExp
    static const QRegularExpression CompleteUrlRegExp;
};

class FilterObject : public QObject
{
Q_OBJECT
public:
    FilterObject(Filter::HotSpot* filter) : _filter(filter) {}
    void request_open_file (const QString& file, int line)
      { Q_EMIT request_open_file_signal (file, line); }
Q_SIGNALS:
    void request_open_file_signal (const QString&, int);
private Q_SLOTS:
    void activated();
private:
    Filter::HotSpot* _filter;
};

/**
 * A chain which allows a group of filters to be processed as one.
 * The chain owns the filters added to it and deletes them when the chain itself is destroyed.
 *
 * Use addFilter() to add a new filter to the chain.
 * When new text to be filtered arrives, use addLine() to add each additional
 * line of text which needs to be processed and then after adding the last line, use
 * process() to cause each filter in the chain to process the text.
 *
 * After processing a block of text, the reset() method can be used to set the filter chain's
 * internal cursor back to the first line.
 *
 * The hotSpotAt() method will return the first hotspot which covers a given position.
 *
 * The hotSpots() and hotSpotsAtLine() method return all of the hotspots in the text and on
 * a given line respectively.
 */
class FilterChain : protected QList<Filter*>
{
public:
    virtual ~FilterChain();

    /** Adds a new filter to the chain.  The chain will delete this filter when it is destroyed */
    void addFilter(Filter* filter);
    /** Removes a filter from the chain.  The chain will no longer delete the filter when destroyed */
    void removeFilter(Filter* filter);
    /** Returns true if the chain contains @p filter */
    bool containsFilter(Filter* filter);
    /** Removes all filters from the chain */
    void clear();

    /** Resets each filter in the chain */
    void reset();
    /**
     * Processes each filter in the chain
     */
    void process();

    /** Sets the buffer for each filter in the chain to process. */
    void setBuffer(const QString* buffer , const QList<int>* linePositions);

    /** Returns the first hotspot which occurs at @p line, @p column or 0 if no hotspot was found */
    Filter::HotSpot* hotSpotAt(int line , int column) const;
    /** Returns a list of all the hotspots in all the chain's filters */
    QList<Filter::HotSpot*> hotSpots() const;
    /** Returns a list of all hotspots at the given line in all the chain's filters */
    QList<Filter::HotSpot> hotSpotsAtLine(int line) const;

};

/** A filter chain which processes character images from terminal displays */
class TerminalImageFilterChain : public FilterChain
{
public:
    TerminalImageFilterChain();
    virtual ~TerminalImageFilterChain();

    /**
     * Set the current terminal image to @p image.
     *
     * @param image The terminal image
     * @param lines The number of lines in the terminal image
     * @param columns The number of columns in the terminal image
     */
    void setImage(const Character* const image , int lines , int columns,
				  const QVector<LineProperty>& lineProperties);

private:
    QString* _buffer;
    QList<int>* _linePositions;
};

#endif //FILTER_H
