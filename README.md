Ubercart Price Per Role
======================

The Ubercart Price-Per-Role module allows Ubercart to charge different prices to
different customers based on their roles.  It works with both base prices and
option prices.

There is also a block that can be used by store administrators to select
a role to use for prices for the current user.

Installation
------------

- Install this module using [the official Backdrop CMS instructions](https://backdropcms.org/guide/modules).

- Visit the configuration page under Administration > Store > Configuration >
Price per Role (admin/store/settings/price-per-role). Here you can select the
roles for individual pricing and set a priority order for the prices. After
setting this up, the new price entry fields will be available from the product
edit pages. When viewing or adding a product to the cart, if a role-specific
price is available and the user has that role, that price will be used. If no
prices are available that match the user's role, the standard product sell price
will be used.

- Administrators may use a price selection block.  This is handy for checking
prices without needing to log in as a user with a particular role.  To use it:

    * Go to the permissions page
    (admin/config/people/permissions#module-uc_price_per_role) and grant access
    to the roles that should use the block.
    * Go to the Layouts administration page (admin/structure/layout), choose the
    layout(s) you would like it to appear on, then add the "Pricing Selection"
    block to the desired region of the layout (usually either the top or bottom
    of the page).
    * Once the block is placed, administrators can use it to select role prices
    that apply to their current browsing session.
        * Note that the admin order creation form does not respect the role of
        the customer for whom the order is being created; it respects the roles
        of the current user and the price selection block.  To create an order
        using specific role pricing, use the price selection block as noted
        above to select the role of the customer, then create the order as
        usual.

- There are Views fields defined for the role ID and role price, also the
"Particular role price," which is useful, for example, if you want to show the
"member price" in addition to the regular price in price lists.

Differences from Drupal 7
-------------------------

* The Price Selection block has an "override" option that allows finer-grained
control over how role pricing is applied to the current user.

* Views support has been added so that you can create lists that contain role
prices.

* Dropped support for the Drupal 7 Migrate module (there is no equivalent in
Backdrop).

Documentation
-------------

Additional documentation is located in [the Wiki](https://github.com/backdrop-contrib/foo-project/wiki/Documentation).

Issues
------

Bugs and feature requests should be reported in [the Issue Queue](https://github.com/backdrop-contrib/foo-project/issues).

Current Maintainers
-------------------

- [Robert J. Lang](https://github.com/bugfolder).

Credits
-------

- Ported to Backdrop CMS by [Robert J. Lang](https://github.com/bugfolder).
- Originally written for Drupal by [Dave Long (longwave)](https://www.drupal.org/u/longwave).

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.

