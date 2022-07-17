# Hexane

This is a HEX editor. It'll be fancy (and I have it written in Python with
all the fancy stuff and now I'm rewriting it to C) and its features will include:

* Vi-like keybindings
* Color-marking
* Advanced search
* Cool commands like selecting a bunch of bytes and incrementing them
* Comments

## File formats

The editor will of course be able to store raw bytes but it'll also have another
format which will include marked and commented bytes which will be useful
for reverse-engineering data.

## Screenshots

The Python version looks like this:
![](https://i.imgur.com/QL31HYl.png)

and this version will look very similar.

## TODO

* Selection
  * Rect selection mode
* Buffer manipulation
  * Removing bytes
  * Inserting bytes
  * Copy/paste
  * Editing bytes
  * Big-endian view
* Search
  * Multiple search queries
  * Navigate through search results
* History
* Comments
* Serialization of metadata

## Dependencies

Aside standard POSIX library, none at the moment.
