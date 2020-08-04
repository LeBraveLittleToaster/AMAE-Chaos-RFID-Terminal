package de.pschiessle.nfcserver

import android.annotation.SuppressLint
import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.TextView
import de.amae.chaos.types.Good

class GoodAdapter(headerTXT: TextView, items: List<Good>, ctx: Context) :
    ArrayAdapter<Good>(ctx, R.layout.good_item, items) {
    private val headerView: TextView = headerTXT;
    private val itemsList: List<Good> = items;
    private class GoodItemViewHolder {
        internal var name: TextView? = null;
        internal var amount: TextView? = null;
        internal var add: Button? = null;
        internal var subtract: Button? = null;
    }

    @SuppressLint("SetTextI18n")
    override fun getView(i: Int, view: View?, viewGroup: ViewGroup): View {
        var view = view;
        val viewHolder: GoodItemViewHolder;

        if (view == null) {
            val inflater = LayoutInflater.from(context);
            view = inflater.inflate(R.layout.good_item, viewGroup, false);
            viewHolder = GoodItemViewHolder();
            viewHolder.name = view!!.findViewById<View>(R.id.name) as TextView;
            viewHolder.amount = view!!.findViewById<View>(R.id.amount) as TextView;
            viewHolder.add = view!!.findViewById<View>(R.id.add) as Button;
            viewHolder.subtract = view!!.findViewById<View>(R.id.subtract) as Button;
        } else {
            viewHolder = view.tag as GoodItemViewHolder;
        }

        val good = getItem(i)
        viewHolder.name!!.text = good!!.name + " | Price: " + String.format("%.2f", (good.priceInCents / 100.0));
        viewHolder.amount!!.text = good!!.amount.toString();
        viewHolder.add!!.setOnClickListener {
            good.amount++;
            viewHolder.amount!!.text = good.amount.toString()
            updateHeader(headerView, computeTotalPrice(itemsList))
        }
        viewHolder.subtract!!.setOnClickListener {
            if (good.amount > 0) {
                good.amount--; viewHolder.amount!!.text = good.amount.toString()
                updateHeader(headerView, computeTotalPrice(itemsList))
            }
        }

        view.tag = viewHolder;
        updateHeader(headerView, computeTotalPrice(itemsList))
        return view;
    }

    @SuppressLint("SetTextI18n")
    private fun updateHeader(header: TextView, amount: Int) {
        header.text = "Total Price: " + String.format("%.2f", (amount / 100.0));
    }

    private fun computeTotalPrice(goods: List<Good>): Int {
        return goods.foldRight(0) { good, sum -> sum + (good.amount * good.priceInCents) };
    }
}