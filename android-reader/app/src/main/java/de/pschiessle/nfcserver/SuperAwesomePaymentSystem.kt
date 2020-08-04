package de.pschiessle.nfcserver

import android.util.Log
import de.amae.chaos.types.Good
import kotlin.random.Random

object SuperAwesomePaymentSystem {
    private var isAwaitingCard: Boolean = false;

    val availableGoods: List<Good> =
        listOf(
            Good("Banane", 120, 0),
            Good("Apfel", 80, 0),
            Good("Joghurt", 140, 0),
            Good("Zahnbürste", 100, 0),
            Good("Kaugummi", 20, 0),
            Good("Starbucks Coffee", 350, 0));

    fun doAwaitCard() {
        isAwaitingCard = true;
    }

    fun confirmPayment(userId: String): Boolean {
        if (isAwaitingCard) {
            isAwaitingCard = false;
            Log.i("PAYMENT", "Sending payment to server! userID=$userId")
            Thread.sleep(500);
            if (true) {
                Log.i("PAYMENT", "Received confirmation for payment! userID=$userId")
                return true;
            }
            Log.i("PAYMENT", "Denied confirmation for payment! userID=$userId")
        }
        return false;
    }

}
